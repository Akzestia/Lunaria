--[[ 
    Use absolute path for "path" properties.
    Configuration options for the application.
--]]
local config = {
    -- User settings
    lang = "en",  -- Language (en | ja | ua)

    -- User avatar settings
    avPath = "",  -- Avatar path
    avBgColor = "",  -- Avatar background color

    -- User banner settings
    bnPath = "",  -- Banner path
    bnBgColor = "",  -- Banner background color

    -- User icon set settings
    osIcons = "",  -- Online status icons
    callIcons = "",  -- Call icons
    sendIcons = "",  -- Send icons

    -- Sign-up window settings
    suFntColPrimary = "#333333",  -- Sign-up window primary font color
    suFntColSecondary = "#666666",  -- Sign-up window secondary font color
    suFntColLink = "#1E90FF",  -- Sign-up window link font color
    suFntColHighlight = "#FFD700",  -- Sign-up window highlighted text color
    suFntSizeSmall = 12,  -- Sign-up window small font size
    suFntSizeMedium = 14,  -- Sign-up window medium font size
    suFntSizeLarge = 18,  -- Sign-up window large font size
    suFntFamily = "Arial, sans-serif",  -- Sign-up window font family
    suOpacity = 0,  -- Sign-up window opacity

    -- Sign-in window settings
    siFntColPrimary = "#333333",  -- Sign-in window primary font color
    siFntColSecondary = "#666666",  -- Sign-in window secondary font color
    siFntColLink = "#1E90FF",  -- Sign-in window link font color
    siFntColHighlight = "#FFD700",  -- Sign-in window highlighted text color
    siFntSizeSmall = 12,  -- Sign-in window small font size
    siFntSizeMedium = 14,  -- Sign-in window medium font size
    siFntSizeLarge = 18,  -- Sign-in window large font size
    siFntFamily = "Arial, sans-serif",  -- Sign-in window font family
    siOpacity = 0,  -- Sign-in window opacity

    -- Main window - Direct messages settings
    mdBgImage = "",  -- Main window direct messages background image
    mdDefaultBgImage = "",  -- Main window direct messages default background image
    mdBgColor = "",  -- Main window direct messages background color
    mdFntColPrimary = "#333333",  -- Main window direct messages primary font color
    mdFntColSecondary = "#666666",  -- Main window direct messages secondary font color
    mdFntColLink = "#1E90FF",  -- Main window direct messages link font color
    mdFntColHighlight = "#FFD700",  -- Main window direct messages highlighted text color
    mdFntSizeSmall = 12,  -- Main window direct messages small font size
    mdFntSizeMedium = 14,  -- Main window direct messages medium font size
    mdFntSizeLarge = 18,  -- Main window direct messages large font size
    mdFntFamily = "Arial, sans-serif",  -- Main window direct messages font family
    mdChatAreaOpacity = 0.8,  -- Main window direct messages chat area opacity
    mdChatAreaBgColor = "",  -- Main window direct messages chat area background color
    mdUserInfoOpacity = 0.8,  -- Main window direct messages user info display opacity
    mdUserInfoBgColor = "white",  -- Main window direct messages user info display background color

    -- Main window - Servers settings
    msBgImage = "",  -- Main window servers background image
    msDefaultBgImage = "",  -- Main window servers default background image
    msBgColor = "",  -- Main window servers background color
    msFntColPrimary = "#333333",  -- Main window servers primary font color
    msFntColSecondary = "#666666",  -- Main window servers secondary font color
    msFntColLink = "#1E90FF",  -- Main window servers link font color
    msFntColHighlight = "#FFD700",  -- Main window servers highlighted text color
    msFntSizeSmall = 12,  -- Main window servers small font size
    msFntSizeMedium = 14,  -- Main window servers medium font size
    msFntSizeLarge = 18,  -- Main window servers large font size
    msFntFamily = "Arial, sans-serif",  -- Main window servers font family
    msChatAreaOpacity = 0.8,  -- Main window servers chat area opacity
    msChatAreaBgColor = "",  -- Main window servers chat area background color
    msUserInfoOpacity = 0.8,  -- Main window servers user info display opacity
    msUserInfoBgColor = ""  -- Main window servers user info display background color
}

return config
