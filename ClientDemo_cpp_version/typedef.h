typedef struct {
    char data[2048];
    size_t len;
}actionAttr;

string s_getcwd()
{
    string res;
    char buffer[2048];
    getcwd(buffer, 2048);
    res = buffer;
    return res;
}