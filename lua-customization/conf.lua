--[[ Use absolute path for "path" prop ]]
local customization = {

    user = {
        language = "en", -- en | ja | ua
        
        avatar = {
            path = "",
            backgroundColor = ""
        },
        banner = {
            path = "",
            backgroundColor = ""
        },
        iconSet = {
            onlineStatusIcons = "",
            callsIcons = "",
            sendIcons = ""
        }
    },

    signUpWindow = {
        font = {
            color = {
                primary = "#333333",  -- Main font color
                secondary = "#666666",  -- Secondary font color
                link = "#1E90FF",  -- Link color
                highlight = "#FFD700"  -- Highlighted text color
            },
            size = {
                small = 12,  -- Small font size
                medium = 14,  -- Medium font size
                large = 18  -- Large font size
            },
            family = "Arial, sans-serif"  -- Font family
        },

        opacity = 0
    },

    signInWindow = {
        font = {
            color = {
                primary = "#333333",  -- Main font color
                secondary = "#666666",  -- Secondary font color
                link = "#1E90FF",  -- Link color
                highlight = "#FFD700"  -- Highlighted text color
            },
            size = {
                small = 12,  -- Small font size
                medium = 14,  -- Medium font size
                large = 18  -- Large font size
            },
            family = "Arial, sans-serif"  -- Font family
        },

        opacity = 0
    },

    mainWindowDirectMessages = {
        backgroundImage = "",
        defaultBackgroundImage = "",
        backgroundColor = "",
        font = {
            color = {
                primary = "#333333",  -- Main font color
                secondary = "#666666",  -- Secondary font color
                link = "#1E90FF",  -- Link color
                highlight = "#FFD700"  -- Highlighted text color
            },
            size = {
                small = 12,  -- Small font size
                medium = 14,  -- Medium font size
                large = 18  -- Large font size
            },
            family = "Arial, sans-serif"  -- Font family
        },
        ContactList = {

        },
        chatArea = {
            opacity = .8,
            backgroundColor = ""
        },
        UserIfnoDisplay = {
            opacity = .8,
            backgroundColor = "white"
        }
    },

    mainWindowServers = {
        backgroundImage = "",
        defaultBackgroundImage = "",
        backgroundColor = "",
        font = {
            color = {
                primary = "#333333",  -- Main font color
                secondary = "#666666",  -- Secondary font color
                link = "#1E90FF",  -- Link color
                highlight = "#FFD700"  -- Highlighted text color
            },
            size = {
                small = 12,  -- Small font size
                medium = 14,  -- Medium font size
                large = 18  -- Large font size
            },
            family = "Arial, sans-serif"  -- Font family
        },
        ContactList = {

        },
        chatArea = {
            opacity = .8,
            backgroundColor = ""
        },
        UserIfnoDisplay = {
            opacity = .8,
            backgroundColor = ""
        }
    }
}

return customization
