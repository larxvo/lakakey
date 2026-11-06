//go:build windows

package main

import "testing"

func TestRuntimeModeSerialization(t *testing.T) {
    s := defaultSettings()
    if got := runtimeValues(s); got != [2]uint32{1, 0} {
        t.Fatalf("default bridge state: %v", got)
    }
    s.Vietnamese = false
    s.Layout = "VNI"
    if got := runtimeValues(s); got != [2]uint32{0, 1} {
        t.Fatalf("mode bridge state: %v", got)
    }
}
