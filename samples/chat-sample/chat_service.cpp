#include "chat_service.hpp"

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(chat, chat_room_subscription, desc)
{
	desc.add_method("user_joined", &chat::chat_room_subscription::user_joined);
	desc.add_method("user_left", &chat::chat_room_subscription::user_left);
	desc.add_method("new_message", &chat::chat_room_subscription::new_message);
}

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(chat, chat_room, desc)
{
	desc.add_method("post_message", &chat::chat_room::post_message);
}

XNET_IMPLEMENT_SERVICE_DESCRIPTOR(chat, root_service, desc)
{
	auto fp = &chat::root_service::chat_rooms;
	desc.add_method("chat_rooms", fp);
	desc.add_method("enter_room", &chat::root_service::enter_room);
}
