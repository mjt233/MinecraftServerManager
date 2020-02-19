typedef unsigned char frame_head_data[5];
class frame_builder{
    public:
        frame_head_data f_data;             //  要发送的帧数据
        unsigned char opcode,               //  操作码
                      FIN;                  //  FIN
        unsigned int length;
        frame_builder(){};
        unsigned char * build()             //  利用当前FIN,opcode和length构造f_data
        {
            build(opcode,length);
            return f_data;
        }

        /**
         * 构造帧
         * @param opcode 操作码
         * @param length 帧数据长度
        */
        unsigned char * build(unsigned char opcode,unsigned int lenth)
        {
            int len = lenth;
            this->length = len;
            FIN = 1;
            f_data[0] = 0x80;
            f_data[0] += opcode & 0x7F;
            memcpy(f_data+1, &len, 4);
            invert((char*)f_data+1,4);
            return f_data;
        }

        /**
         * 解析帧,解析后更新对象对应的属性
         * @param data 数据帧
        */
        void parse(const frame_head_data data)
        {
            FIN =( data[0] & 0x80 ) == 0x80;
            opcode = data[0] & 0x7F;
            memcpy( &length, data + 1, 4);
            invert((char*)&length,4);
        }
};