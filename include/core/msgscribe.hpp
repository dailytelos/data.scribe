

public:
ACTION message(name sender, string msg);

ACTION msgid(uint128_t id, name sender);

private:
void msg_proc(name sender, string msg);
void msg_broadcast_id(name sender);


