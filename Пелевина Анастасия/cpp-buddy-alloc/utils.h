size_t strlen(char* word) {
    auto b = word;
    while (*b != '\0')
        b++;

    return b - word;
}

class MyCompare
{
public:
    bool operator()(char *a, char *b) const {
        while (isalpha(*a)) {
            char low_a = tolower(*a);
            char low_b = tolower(*b);
            if (low_a < low_b) {
                return true;
            }
            if (low_a > low_b) {
                return false;
            }

            a++;
            b++;
        }

        return isalpha(*b);
    }
};
