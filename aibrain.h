#ifndef BRAINZ
#define BRAINZ
#include <QObject>
#include <QScriptEngine>
#include <QScriptValue>
#include <QList>
#include <QPoint>
#include <QThread>
#include <QMutex>

struct Move { int x1, x2, y1, y2, value; };

class GameState {
    public:
    GameState();

    GameState(GameState &other);
    void clone(GameState &other);

    int get(int x, int y);
    void set(int x, int y, int val);

    int gameOver();

    //Board is mapped to a grid, insert spaces per row as needed in accessors
    int lastMoveX;
    int lastMoveY;

    int board[4*8];//###Perhaps this should be a 2D array, but I needed an idx function anyways
};

class ThinkThread;
class AIBrain : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int depth READ depth WRITE setDepth NOTIFY depthChanged)
    Q_PROPERTY(bool thinking READ thinking NOTIFY thinkingChanged) //For UI hints, but currently there's no repaint in between...
    Q_PROPERTY(QVariantMap move READ move NOTIFY moveChanged)
public:
    int depth() const { return m_maxDepth; }
    QVariantMap move() const { return m_move; }
    void setDepth(int v) { if(v==m_maxDepth) return; m_maxDepth = v; emit depthChanged(); }
    bool thinking() const { return m_thinking; }
    AIBrain(QObject* parent = 0);

    //IN: Array of pieces. OUT: Object with pieceRow/Col targetRow/Col (in the move property)
    Q_INVOKABLE void thinkHard(QVariantList pieces);

protected slots:
    void finishMove();
signals:
    void depthChanged();
    void thinkingChanged();
    void moveChanged();

private:
    int m_maxDepth;
    bool m_thinking;
    QVariantMap m_move;
    ThinkThread* m_thread;
};

class ThinkThread : public QThread
{
public:
    ThinkThread();
    virtual void run();
    QMutex aMutex;

    GameState* startingState;
    int depth;

    QVariantMap result;
};
#endif
