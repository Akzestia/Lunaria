



class ClientListener{
    public:

    protected:
        virtual ~ClientListener();
    private:
        ClientListener() = default;
        friend class QuicClient;
};