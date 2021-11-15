#include "sqlsyntaxhighlighter.h"

SQLSyntaxHighlighter::SQLSyntaxHighlighter(QTextDocument *parent) : QSyntaxHighlighter(parent)
{
    HighlightingRule rule;

    functionFormat.setFontItalic(true);
    functionFormat.setForeground(Qt::blue);
    rule.pattern = QRegularExpression(QStringLiteral("\\b[A-Za-z0-9_]+(?=\\()"));
    rule.format = functionFormat;
    highlightingRules.append(rule);

    keywordFormat.setForeground(Qt::darkBlue);
    keywordFormat.setFontWeight(QFont::Bold);
    const QString keywordPatterns[] = {
        QStringLiteral("\\bADD\\b"),
        QStringLiteral("\\bADD CONSTRAINT\\b"),
        QStringLiteral("\\bALTER\\b"),
        QStringLiteral("\\bALTER COLUMN\\b"),
        QStringLiteral("\\bALTER TABLE\\b"),
        QStringLiteral("\\bALL\\b"),
        QStringLiteral("\\bAND\\b"),
        QStringLiteral("\\bANY\\b"),
        QStringLiteral("\\bAS\\b"),
        QStringLiteral("\\bASC\\b"),
        QStringLiteral("\\bBACKUP DATABASE\\b"),
        QStringLiteral("\\bBETWEEN\\b"),
        QStringLiteral("\\bCASE\\b"),
        QStringLiteral("\\bCHECK\\b"),
        QStringLiteral("\\bCOLUMN\\b"),
        QStringLiteral("\\bCONSTRAINT\\b"),
        QStringLiteral("\\bCREATE\\b"),
        QStringLiteral("\\bCREATE DATABASE\\b"),
        QStringLiteral("\\bCREATE INDEX\\b"),
        QStringLiteral("\\bCREATE OR REPLACE VIEW\\b"),
        QStringLiteral("\\bCREATE TABLE\\b"),
        QStringLiteral("\\bCREATE PROCEDURE\\b"),
        QStringLiteral("\\bCREATE UNIQUE INDEX\\b"),
        QStringLiteral("\\bCREATE VIEW\\b"),
        QStringLiteral("\\bDATABASE\\b"),
        QStringLiteral("\\bDEFAULT\\b"),
        QStringLiteral("\\bDELETE\\b"),
        QStringLiteral("\\bDESC\\b"),
        QStringLiteral("\\bDISTINCT\\b"),
        QStringLiteral("\\bDROP\\b"),
        QStringLiteral("\\bDROP COLUMN\\b"),
        QStringLiteral("\\bDROP CONSTRAINT\\b"),
        QStringLiteral("\\bDROP DATABASE\\b"),
        QStringLiteral("\\bDROP DEFAULT\\b"),
        QStringLiteral("\\bDROP INDEX\\b"),
        QStringLiteral("\\bDROP TABLE\\b"),
        QStringLiteral("\\bDROP VIEW\\b"),
        QStringLiteral("\\bEXEC\\b"),
        QStringLiteral("\\bEXISTS\\b"),
        QStringLiteral("\\bFOREIGN KEY\\b"),
        QStringLiteral("\\bFROM\\b"),
        QStringLiteral("\\bFULL OUTER JOIN\\b"),
        QStringLiteral("\\bGROUP BY\\b"),
        QStringLiteral("\\bHAVING\\b"),
        QStringLiteral("\\bIN\\b"),
        QStringLiteral("\\bINDEX\\b"),
        QStringLiteral("\\bINNER JOIN\\b"),
        QStringLiteral("\\bINSERT INTO\\b"),
        QStringLiteral("\\bINSERT INTO SELECT\\b"),
        QStringLiteral("\\bIS NULL\\b"),
        QStringLiteral("\\bIS NOT NULL\\b"),
        QStringLiteral("\\bJOIN\\b"),
        QStringLiteral("\\bLEFT JOIN\\b"),
        QStringLiteral("\\bLIKE\\b"),
        QStringLiteral("\\bLIMIT\\b"),
        QStringLiteral("\\bNOT\\b"),
        QStringLiteral("\\bNOT NULL\\b"),
        QStringLiteral("\\bOR\\b"),
        QStringLiteral("\\bORDER BY\\b"),
        QStringLiteral("\\bOUTER JOIN\\b"),
        QStringLiteral("\\bPRIMARY KEY\\b"),
        QStringLiteral("\\bPROCEDURE\\b"),
        QStringLiteral("\\bRIGHT JOIN\\b"),
        QStringLiteral("\\bROWNUM\\b"),
        QStringLiteral("\\bSELECT\\b"),
        QStringLiteral("\\bSELECT DISTINCT\\b"),
        QStringLiteral("\\bSELECT INTO\\b"),
        QStringLiteral("\\bSELECT TOP\\b"),
        QStringLiteral("\\bSET\\b"),
        QStringLiteral("\\bTABLE\\b"),
        QStringLiteral("\\bTOP\\b"),
        QStringLiteral("\\bTRUNCATE TABLE\\b"),
        QStringLiteral("\\bUNION\\b"),
        QStringLiteral("\\bUNION ALL\\b"),
        QStringLiteral("\\bUNIQUE\\b"),
        QStringLiteral("\\bUPDATE\\b"),
        QStringLiteral("\\bVALUES\\b"),
        QStringLiteral("\\bVIEW\\b"),
        QStringLiteral("\\bWHEN\\b"),
        QStringLiteral("\\bTHEN\\b"),
        QStringLiteral("\\bWITH\\b"),
        QStringLiteral("\\b(NOLOCK)\\b"),
        QStringLiteral("\\b(ROWLOCK)\\b"),
        QStringLiteral("\\bON\\b"),
        QStringLiteral("\\bIF\\b"),
        QStringLiteral("\\bDECLARE\\b"),
        QStringLiteral("\\bELSE\\b"),
        QStringLiteral("\\bEND\\b"),
        QStringLiteral("\\bINSERT\\b"),
        QStringLiteral("\\bGO\\b"),
        QStringLiteral("\\bWHERE\\b")
    };
    for (const QString &pattern : keywordPatterns) {
        rule.pattern = QRegularExpression(pattern, QRegularExpression::CaseInsensitiveOption);
        rule.format = keywordFormat;
        highlightingRules.append(rule);
    }

    quotationFormat.setForeground(Qt::darkGreen);
    rule.pattern = QRegularExpression(QStringLiteral("\'.*?\'"));
    rule.format = quotationFormat;
    highlightingRules.append(rule);

    singleLineCommentFormat.setForeground(Qt::darkGray);
    rule.pattern = QRegularExpression(QStringLiteral("--[^\n]*"));
    rule.format = singleLineCommentFormat;
    highlightingRules.append(rule);

    multiLineCommentFormat.setForeground(Qt::darkGray);


    commentStartExpression = QRegularExpression(QStringLiteral("/\\*"));
    commentEndExpression = QRegularExpression(QStringLiteral("\\*/"));
}

SQLSyntaxHighlighter::~SQLSyntaxHighlighter()
{
    delete &highlightingRules;
    delete &commentStartExpression;
    delete &commentEndExpression;
    delete &keywordFormat;
    delete &classFormat;
    delete &singleLineCommentFormat;
    delete &multiLineCommentFormat;
    delete &quotationFormat;
    delete &functionFormat;
}

void SQLSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : qAsConst(highlightingRules)) {
        QRegularExpressionMatchIterator matchIterator = rule.pattern.globalMatch(text);
        while (matchIterator.hasNext()) {
            QRegularExpressionMatch match = matchIterator.next();
            setFormat(match.capturedStart(), match.capturedLength(), rule.format);
        }
    }
    setCurrentBlockState(0);

    int startIndex = 0;
    if (previousBlockState() != 1)
        startIndex = text.indexOf(commentStartExpression);

    while (startIndex >= 0) {
        QRegularExpressionMatch match = commentEndExpression.match(text, startIndex);
        int endIndex = match.capturedStart();
        int commentLength = 0;
        if (endIndex == -1) {
            setCurrentBlockState(1);
            commentLength = text.length() - startIndex;
        } else {
            commentLength = endIndex - startIndex
                            + match.capturedLength();
        }
        setFormat(startIndex, commentLength, multiLineCommentFormat);
        startIndex = text.indexOf(commentStartExpression, startIndex + commentLength);
    }
}
