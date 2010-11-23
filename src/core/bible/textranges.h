#ifndef TEXTRANGES_H
#define TEXTRANGES_H
#include <QtCore/QList>
#include "src/core/bible/textrange.h"
class TextRanges
{
public:
    TextRanges();
    void addTextRange(const TextRange &range);
    QList<TextRange> textRanges();
private:
    QList<TextRange> m_ranges;
};

#endif // TEXTRANGES_H