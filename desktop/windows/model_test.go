package main

import (
	"os"
	"path/filepath"
	"testing"
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
