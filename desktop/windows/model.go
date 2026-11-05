package main

import (
	"encoding/json"
	"os"
	"path/filepath"
)

// Settings are local-only. This is a desktop shell; it does not enable a system IME.
type Settings struct {
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
	return Settings{Layout: "Telex", Encoding: "Unicode", ToggleKey: "Alt+Z", Vietnamese: true, Mascot: "Off", ShowAtStartup: true}
}
func normalize(s Settings) Settings {
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
func readSettings() Settings {
	raw, err := os.ReadFile(configPath())
	if err != nil {
		return defaultSettings()
	}
	s := defaultSettings()
	if json.Unmarshal(raw, &s) != nil {
		return defaultSettings()
	}
	return normalize(s)
}
func writeSettings(s Settings) error {
	path := configPath()
	if err := os.MkdirAll(filepath.Dir(path), 0700); err != nil {
		return err
	}
	bytes, err := json.MarshalIndent(normalize(s), "", "  ")
	if err != nil {
		return err
	}
	temp := path + ".tmp"
	if err = os.WriteFile(temp, bytes, 0600); err != nil {
		return err
	}
	// Windows replacement semantics need the old file removed before rename.
	_ = os.Remove(path)
	return os.Rename(temp, path)
}
