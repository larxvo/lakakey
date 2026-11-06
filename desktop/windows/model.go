package main

import (
	"encoding/json"
	"fmt"
	"os"
	"path/filepath"
)

// Settings are local-only. This is a desktop shell; it does not enable a system IME.
const settingsSchemaVersion = 2

type Settings struct {
	SchemaVersion int `json:"schema_version"`
	Layout           string `json:"layout"`
	Encoding         string `json:"encoding"`
	ToggleKey        string `json:"toggle_key"`
	Vietnamese       bool   `json:"vietnamese"`
	Expanded         bool   `json:"expanded"`
	StartWithWindows bool   `json:"start_with_windows"`
	ShowAtStartup    bool   `json:"show_at_startup"`
	PerAppMode       bool   `json:"per_app_mode"`
	Spellcheck       bool   `json:"spellcheck"`
	FreeTyping       bool   `json:"free_typing"`
	ModernTone       bool   `json:"modern_tone"`
	Shorthand        bool   `json:"shorthand"`
	ShorthandEnglish bool   `json:"shorthand_english"`
	Mascot           string `json:"mascot"`
}

var inputLayouts = []string{"Telex", "Telex giản lược (chưa hỗ trợ)", "VNI", "VIQR (chưa hỗ trợ)", "Microsoft (chưa hỗ trợ)", "Tự định nghĩa (chưa hỗ trợ)"}
var encodings = []string{"Unicode", "TCVN3 (chưa hỗ trợ)", "VNI Windows (chưa hỗ trợ)", "VIQR (chưa hỗ trợ)", "CP 1258 (chưa hỗ trợ)", "Unicode tổ hợp (chưa hỗ trợ)", "UTF-8 Literal (chưa hỗ trợ)", "NCR Decimal (chưa hỗ trợ)", "NCR Hex (chưa hỗ trợ)", "Unicode C String (chưa hỗ trợ)"}

func defaultSettings() Settings {
	return Settings{SchemaVersion:settingsSchemaVersion, Layout: "Telex", Encoding: "Unicode", ToggleKey: "Alt+Z", Vietnamese: true, Mascot: "Off", ShowAtStartup: false}
}
func normalize(s Settings) Settings {
	s.SchemaVersion=settingsSchemaVersion
	s.PerAppMode=false; s.Spellcheck=false; s.FreeTyping=false; s.ModernTone=false; s.Shorthand=false; s.ShorthandEnglish=false
	if s.Layout != "Telex" && s.Layout != "VNI" {
		s.Layout = "Telex"
	}
	if s.Encoding != "Unicode" {
		s.Encoding = "Unicode"
	}
	if s.ToggleKey != "Alt+Z" {
		s.ToggleKey = "Alt+Z"
	}
	if s.Mascot != "Full" && s.Mascot != "Minimal" && s.Mascot != "Off" {
		s.Mascot = "Off"
	}
	return s
}
func configPath() string {
	base := os.Getenv("APPDATA")
	if base == "" {
		base, _ = os.UserConfigDir()
	}
	if base == "" {
		base = os.TempDir()
	}
	return filepath.Join(base, "Lakakey", "settings.json")
}
func parseSettings(raw []byte) (Settings,error) {
  if len(raw)==0 {return Settings{},fmt.Errorf("empty settings")}
  s:=defaultSettings()
  if err:=json.Unmarshal(raw,&s);err!=nil{return Settings{},err}
  if s.SchemaVersion>settingsSchemaVersion {return Settings{},fmt.Errorf("future settings schema %d",s.SchemaVersion)}
  return normalize(s),nil
}
func readSettings() Settings {
  path:=configPath()
  for _,candidate:=range []string{path,path+".bak"} {
    raw,err:=os.ReadFile(candidate)
    if err!=nil{continue}
    if s,err:=parseSettings(raw);err==nil{return s}
  }
  return defaultSettings()
}
func atomicWriteFile(path string, data []byte) error {
  dir:=filepath.Dir(path)
  if err:=os.MkdirAll(dir,0700);err!=nil{return err}
  f,err:=os.CreateTemp(dir,".lakakey-*.tmp")
  if err!=nil{return err}
  tmp:=f.Name()
  defer os.Remove(tmp)
  if err=f.Chmod(0600);err!=nil{f.Close();return err}
  if _,err=f.Write(data);err!=nil{f.Close();return err}
  if err=f.Sync();err!=nil{f.Close();return err}
  if err=f.Close();err!=nil{return err}
  return replaceFileAtomic(tmp,path)
}
func writeSettings(s Settings) error {
  path:=configPath()
  old,err:=os.ReadFile(path)
  if err==nil {
    var meta struct{SchemaVersion int `json:"schema_version"`}
    if json.Unmarshal(old,&meta)==nil && meta.SchemaVersion>settingsSchemaVersion {
      return fmt.Errorf("cannot overwrite newer settings schema %d",meta.SchemaVersion)
    }
    if _,err:=parseSettings(old);err==nil { _=atomicWriteFile(path+".bak",old) }
  }
  data,err:=json.MarshalIndent(normalize(s),"","  ")
  if err!=nil{return err}
  return atomicWriteFile(path,data)
}
