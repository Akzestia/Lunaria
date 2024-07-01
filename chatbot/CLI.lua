function respond(command)
    if string.lower(command) == "hello" then
        return "Hello! How can I help you?"
    elseif string.lower(command) == "bye" then
        return "Goodbye!"
    else
        return "Sorry, I don't understand."
    end
end
