package main

import (
	"os"
	"path/filepath"
	"testing"
"strings"
)

func TestNormalize(t *testing.T) {
	x := normalize(Settings{Layout: "Microsoft", Encoding: "TCVN3", ToggleKey: "something", Mascot: "floating"})
	if x.Layout != "Telex" || x.Encoding != "Unicode" || x.ToggleKey != "Alt+Z" || x.Mascot != "Off" {
		t.Fatalf("unsafe settings accepted: %#v", x)
	}
	y := normalize(Settings{Layout: "VNI", Encoding: "Unicode", ToggleKey: "Alt+Z", Mascot: "Minimal"})
	if y.Layout != "VNI" || y.ToggleKey != "Alt+Z" || y.Mascot != "Minimal" {
		t.Fatalf("valid settings rejected: %#v", y)
	}
}
func TestPersistSettings(t *testing.T) {
	d := t.TempDir()
	t.Setenv("APPDATA", d)
	s := defaultSettings()
	s.Layout = "VNI"
	s.Expanded = true
	s.Vietnamese = false
	if e := writeSettings(s); e != nil {
		t.Fatal(e)
	}
	if _, e := os.Stat(filepath.Join(d, "Lakakey", "settings.json")); e != nil {
		t.Fatal(e)
	}
	got := readSettings()
	if got.Layout != "VNI" || !got.Expanded || got.Vietnamese {
		t.Fatalf("wrong roundtrip: %#v", got)
	}
}

func TestRecoverCorruptSettings(t *testing.T) {
  t.Setenv("APPDATA", t.TempDir())
  s:=defaultSettings();s.Layout="VNI"
  if err:=writeSettings(s);err!=nil{t.Fatal(err)}
  s.Layout="Telex";if err:=writeSettings(s);err!=nil{t.Fatal(err)}
  if err:=os.WriteFile(configPath(),[]byte("{invalid"),0600);err!=nil{t.Fatal(err)}
  if got:=readSettings();got.Layout!="VNI"{t.Fatalf("backup recovery failed: %#v",got)}
}
func TestLegacySchemaMigration(t *testing.T){
  s,err:=parseSettings([]byte(`{"layout":"VNI","encoding":"Unicode","vietnamese":false}`))
  if err!=nil||s.Layout!="VNI"||s.Vietnamese||s.SchemaVersion!=settingsSchemaVersion{t.Fatalf("migration: %v %#v",err,s)}
}
func TestFutureSettingsPreserved(t *testing.T){
  t.Setenv("APPDATA",t.TempDir());p:=configPath()
  if err:=os.MkdirAll(filepath.Dir(p),0700);err!=nil{t.Fatal(err)}
  payload:=[]byte(`{"schema_version":999,"layout":"future"}`)
  if err:=os.WriteFile(p,payload,0600);err!=nil{t.Fatal(err)}
  if err:=writeSettings(defaultSettings());err==nil{t.Fatal("future schema overwritten")}
  after,err:=os.ReadFile(p)
  if err!=nil||string(after)!=string(payload){t.Fatalf("data loss on downgrade: %v",err)}
}
func TestAtomicReplacementFailure(t *testing.T) {
  dir:=t.TempDir();target:=filepath.Join(dir,"directory")
  if err:=os.Mkdir(target,0700);err!=nil{t.Fatal(err)}
  if err:=atomicWriteFile(target,[]byte("new"));err==nil{t.Fatal("replaced a directory")}
  items,_:=os.ReadDir(dir)
  for _,item:=range items{if strings.HasSuffix(item.Name(),".tmp"){t.Errorf("orphan temporary file %s",item.Name())}}
}
func TestRepeatedWritesAndBackup(t *testing.T) {
  t.Setenv("APPDATA",t.TempDir());s:=defaultSettings()
  for i:=0;i<30;i++ {s.Vietnamese=i%2==0;if err:=writeSettings(s);err!=nil{t.Fatal(err)}}
  if _,err:=os.Stat(configPath()+".bak");err!=nil{t.Fatalf("missing backup: %v",err)}
  if got:=readSettings();!got.Vietnamese{t.Fatalf("latest write lost: %#v",got)}
}
