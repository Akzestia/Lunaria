## Lua customization scripts



```
-- Customization settings for the chat application

local customization = {
    -- Font settings
    font = {
        color = {
            primary = "#333333",  -- Main font color
            secondary = "#666666",  -- Secondary font color
            link = "#1E90FF",  -- Link color
            highlight = "#FFD700"  -- Highlighted text color
        
        size = {
            small = 12,  -- Small font size
            medium = 14,  -- Medium font size
            large = 18  -- Large font size
        },
        family = "Arial, sans-serif"  -- Font family
    },

    -- Background settings
    background = {
        main = "#FFFFFF",  -- Main background color
        sidebar = "#F4F4F4",  -- Sidebar background color
        chatWindow = "#E9E9E9",  -- Chat window background color
        inputBox = "#FFFFFF"  -- Input box background color
    },

    -- Banner settings
    banner = {
        image = "/",  -- Path to the banner image
        color = "#333333",  -- Banner background color if no image is used
        height = 150  -- Height of the banner in pixels
    },

    -- Avatar settings
    avatar = {
        default = "/",  -- Path to the default avatar image
        size = {
            small = 30,  -- Small avatar size in pixels
            medium = 50,  -- Medium avatar size in pixels
            large = 70  -- Large avatar size in pixels
        },
        shape = "circle"  -- Shape of the avatar (e.g., "circle", "square")
    },

    -- Other UI elements
    ui = {
        buttonColor = "#1E90FF",  -- Color of buttons
        borderColor = "#CCCCCC",  -- Border color for elements
        shadow = true  -- Enable or disable shadow effects
    }
}

return customization

```