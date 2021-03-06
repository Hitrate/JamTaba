#include <QApplication>
#include <QFrame>
#include <QPaintEvent>
#include <QShowEvent>
#include <QDebug>
#include <QTimer>
#include <QTimerEvent>
#include <QTime>
#include <QHBoxLayout>
#include <cmath>
#include <gui/widgets/PeakMeter.h>

/**
 * This test is stressing the PeakMeter paint and plotting the fps (frames per second).
 * The idea is build a benchmark to compare fps values when optmizing the PeakMeter painting.
 */

class TestMainWindow : public QFrame
{

public:
    TestMainWindow()
        :QFrame()
    {
        refreshTimerID = startTimer(0);
        frameCount = 0;
        timeCounter.start();

        //create meters
        const int METERS = 64;
        const int METER_WIDTH = 5;
        QHBoxLayout *layout = new QHBoxLayout();
        for (int var = 0; var < METERS; ++var) {
            AudioMeter *meter = new AudioMeter(this);
            meter->setMinimumWidth(METER_WIDTH);
            layout->addWidget(meter, 1);
        }
        setLayout(layout);

        setMinimumWidth(METERS * METER_WIDTH);
        setMinimumHeight(400);

    }

    ~TestMainWindow()
    {
        killTimer(refreshTimerID);
    }

protected:

    void paintEvent(QPaintEvent *ev) override
    {
        QFrame::paintEvent(ev);
        frameCount++;

        if(timeCounter.elapsed() >= 1000){
            int fps = frameCount;
            qInfo() << "FPS:" << fps;
            timeCounter.restart();
            frameCount = 0;
        }
    }

    void timerEvent(QTimerEvent *) override
    {
        static float peak = 0;
        QList<AudioMeter *> meters = findChildren<AudioMeter *>();
        foreach (AudioMeter *meter, meters) {
            meter->setPeak(peak, peak);
        }
        peak += 0.00001f;
        //peak = 0.11f;
        if(peak >= 1)
            peak = 0;
        update();
    }

private:
    int refreshTimerID;
    QTime timeCounter;
    int frameCount;
};


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    TestMainWindow window;

    window.show();
    return app.exec();
}
