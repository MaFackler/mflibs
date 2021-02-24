TEST("CONDITION")
{
    MFT_CHECK(1 == 1);
}

TEST("PTR")
{
    int a = 2;
    int *aa = &a;
    int *ab = &a;
    MFT_CHECK_PTR(aa, ab);
}

TEST("FLOAT")
{
    MFT_CHECK_FLOAT(1.0f, 1.00000001f);
}

TEST("CHAR")
{
    MFT_CHECK_CHAR('a', 'a');
}

TEST("INT")
{
    MFT_CHECK_INT(1, 1);
}


TEST("STRING")
{
    char buffer[256];
    buffer[0] = 'a';
    buffer[1] = 'b';
    buffer[2] = 'c';
    MFT_CHECK_STRINGN(&buffer[0], "abc", 3);

    buffer[3] = 0;
    MFT_CHECK_STRING(&buffer[0], "abc");
}

