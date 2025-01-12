#ifndef EKRAN_H
#define EKRAN_H

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPainter>
#include <QMouseEvent>
#include <QPushButton>
#include <QCheckBox>

#include <array>

struct PixelColor
{
    int R;
    int G;
    int B;
};


class Ekran : public QWidget
{
    Q_OBJECT
public:
    explicit Ekran(QWidget *parent = nullptr);
    //~Ekran();

protected:

    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:

    enum class InterpolationMode{
        OFF = 0, ON
    };

    void setupUI();
    void drawPixel(QImage& img, int x, int y, int h_color);
    void drawLineBresenham(QImage& img, const QPoint& first, const QPoint& second);

    void drawTriangle(const std::array<QPoint, 3>& points, const std::array<QPoint, 3>& texturePoints, InterpolationMode mode);


    PixelColor getPixel(const QImage& img, int x, int y) const;
    void setPixel(QImage& img, int x, int y, const PixelColor& color);
    void drawCircle(QImage &img, const QPoint &first, float r);

private:
    QImage m_texture;
    QImage m_temp;
    QImage m_canvas;

    std::array<QPoint, 3> m_canvasPoints;
    std::array<QPoint, 3> m_texturePoints;
    QPoint m_canvasStartPoint;
    QPoint m_textureStartPoint;
    int m_index;
    bool m_isDrawing;
    InterpolationMode m_mode;



    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_buttonsLayout;
    QVBoxLayout* m_imgLayout;
    QPushButton* m_resetButton;
    QCheckBox* m_interpolationCheckBox;
    QPushButton* m_textureButton;
    QWidget* m_imgPanel;
    QSpacerItem* m_spacer;
    QSpacerItem* m_spacer2;


private slots:

    void reset();
    void setInterpolationMode();

signals:
};

#endif // EKRAN_H
