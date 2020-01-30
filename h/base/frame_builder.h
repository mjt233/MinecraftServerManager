class frame_builder{
    public:
        frame_head_data f_data;
        unsigned char opcode,FIN;
        unsigned int length;
        frame_builder(){};
        frame_builder(unsigned char opcode,unsigned int lenth)
        {
            build(opcode, length);
        }
        unsigned char * build()
        {
            build(opcode,length);
            return f_data;
        }
        unsigned char * build(unsigned char opcode,unsigned int lenth)
        {
            f_data[0] = 0x80;
            f_data[0] += opcode & 0x7F;
            memcpy(f_data+1, &length, 4);
            invert((char*)f_data+1,4);
            return f_data;
        }
        void analyze(const frame_head_data data)
        {
            FIN =( data[0] & 0x80 ) == 0x80;
            opcode = data[0] & 0x7F;
            memcpy( &length, data + 1, 4);
            invert((char*)&length,4);
        }
};