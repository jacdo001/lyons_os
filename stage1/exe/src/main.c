void print_call(char c)
{
    __asm__("mov $0x01, %%eax; mov %0, %%ebx; int $0x80;"
        :
        : "m" (c)
        : "eax", "ebx");
}

void print_string(char* s)
{
    char *p = s;
    while(*p){ print_call(*(p++)); }
}

void test_call()
{
    __asm__("mov $0x00, %eax; int $0x80;");
}

int main(int argc, char **argv)
{
    test_call();
    print_string("Hello, user world!");
    while(1);
}
