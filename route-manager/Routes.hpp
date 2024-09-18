#include <cstdint>

//todo
enum Routes : uint8_t {
    SIGN_UP = 0x01,
    SIGN_IN = 0x02,
    SEND_FRIEND_REQUEST = 0x03,
    REMOVE_FRIEND = 0x04,
    SEND_MESSAGE = 0x05,
    RECEIVE_MESSAGE = 0x06,
    CREATE_CHAT_GROUP = 0x07,
    JOIN_CHAT_GROUP = 0x08,
    LEAVE_CHAT_GROUP = 0x09,
    ADD_GROUP_MEMBER = 0x0A,
    REMOVE_GROUP_MEMBER = 0x0B,
    TYPING_INDICATOR = 0x0C,
    READ_RECEIPT = 0x0D,
    DELETE_MESSAGE = 0x0E,
    EDIT_MESSAGE = 0x0F,
    FETCH_CHAT_HISTORY = 0x10,
    USER_ONLINE_STATUS = 0x11,
    USER_OFFLINE_STATUS = 0x12,
    AUTH_RESPONSE_SIGN_UP = 0x13,
    AUTH_RESPONSE_SIGN_IN = 0x14,
    SERVER_BINDING_REQUEST = 0x15,
    CREATE_CONTACT = 0x16,
    POST_RESPONSE_CONTACT = 0x17,
    FETCH_CONTACTS_BY_ID = 0x18,
    FETCH_CONTACTS_RESPONSE = 0x19,
    FETCH_DM_MESSAGES = 0x20,
    FETCH_CHANNEL_MESSAGES = 0x2A,
    SEND_MESSAGE_TO_USER = 0x2B,
    SEND_MESSAGE_TO_USER_RESPONSE = 0x2C,
    FETCH_DM_MESSAGES_RESPONSE = 0x2D
};
