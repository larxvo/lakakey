# macOS native adapter — pending
Use InputMethodKit / IMKInputController for composition and commit; do not inject synthetic key events. Provide a signed and notarized .app input method bundle, with correct Info.plist and secure-field bypass. The portable `lakakey::InputSession` is ready for use but is not a working macOS IME.
