#ifndef MMU_H
#define MMU_H

class c_MMU {
    public:
        c_MMU();
        ~c_MMU();
        void LoadROM(const char* fname);
};

#endif // MMU_H
