& $PSScriptRoot/build.ps1
if ($?) {
    adb push libs/arm64-v8a/libquestui_components_unused.so /sdcard/Android/data/com.beatgames.beatsaber/files/mods/libquestui_components_unused.so
    if ($?) {
        & $PSScriptRoot/restart-game.ps1
        if ($args[0] -eq "--log") {
            & $PSScriptRoot/start-logging.ps1
        }
    }
}
