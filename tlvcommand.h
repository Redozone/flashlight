#ifndef TLVCOMMAND_H
#define TLVCOMMAND_H

#include <QColor>

class TLVcommand
{
public:
    enum pufCommand {
       ON    = 0x12,
       OFF   = 0x13,
       COLOR = 0x20,
       UNKN  = 0xFF
    };

    TLVcommand();
    ~TLVcommand();
    pufCommand type() const;
    void setType(pufCommand type);

    int lenght() const;
    void setLenght(int lenght);

    char value() const;
    void setValue(char value);

    QColor color() const;
    void setColor(QColor color);


private:
    int m_type;
    int m_lenght;
    char m_value;
    QColor m_color;
};

#endif // TLVCOMMAND_H
