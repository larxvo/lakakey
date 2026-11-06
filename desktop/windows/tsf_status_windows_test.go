//go:build windows

package main

import (
    "strings"
    "testing"
)

func TestInlineNativeDiagnostics(t *testing.T) {
    message := nativeStatusText()
    if !strings.Contains(message, "Hồ sơ Windows TSF") {
        t.Fatalf("missing native status description: %q", message)
    }
    if !strings.Contains(message, "Không ghi lại nội dung gõ") {
        t.Fatal("privacy description missing")
    }
    if strings.Contains(message, "đã xác minh gõ toàn hệ thống") {
        t.Fatal("do not present an experimental adapter as verified")
    }
}
