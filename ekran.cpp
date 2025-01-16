#include "ekran.h"

Ekran::Ekran(QWidget *parent)
    : QWidget{parent}, m_index(0), m_isDrawing(false), m_mode(InterpolationMode::OFF)
{
    m_texture = QImage("Assets/Obrazek1.jpg");
    m_canvas = QImage(560, 640, QImage::Format_RGB32);
    m_canvas.fill(0);
    m_temp = m_texture.copy();

    resize(800, 600);
    setupUI();
}

void Ekran::paintEvent(QPaintEvent *event)
{
    QPainter p(this);

    if (!m_texture.isNull() && !m_canvas.isNull())
    {
        p.drawImage(QPoint(0,0), m_texture);
        p.drawImage(QPoint(400, 0), m_canvas);
    }
}

void Ekran::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton || Qt::RightButton)
    {
        m_isDrawing = true;
    }
}

void Ekran::mouseReleaseEvent(QMouseEvent *event)
{

    if (event->button() == Qt::RightButton && m_isDrawing)
    {

        QPoint point = event->pos();
        m_canvasPoints[m_index] = {point.x() - 400, point.y()};
        m_index++;


        drawCircle(m_canvas, {point.x() - 400, point.y()}, 3);

        if (m_index == 3)
        {
            m_canvasStartPoint = m_canvasPoints[0];

            for (int i = 0; i < m_index - 1; i++)
            {
                if (point.x() >= 400 && point.x() <= 1000)
                {
                    drawLineBresenham(m_canvas, m_canvasPoints[i], m_canvasPoints[i + 1]);
                    drawLineBresenham(m_canvas, m_canvasPoints[2], m_canvasStartPoint);
                }
            }
            m_index = 0;
        }
        m_isDrawing = false;
        update();
    }

    if (event->button() == Qt::LeftButton && m_isDrawing)
    {

        QPoint point = event->pos();
        m_texturePoints[m_index] = point;
        m_index++;

        drawCircle(m_texture, point, 3);

        if (m_index == 3)
        {
            m_textureStartPoint = m_texturePoints[0];

            for (int i = 0; i < m_index - 1; i++)
            {
                if (point.x() >= 0 && point.x() <= 560)
                {
                    drawLineBresenham(m_texture, m_texturePoints[i], m_texturePoints[i + 1]);
                    drawLineBresenham(m_texture, m_texturePoints[2], m_textureStartPoint);
                }
            }
            m_index = 0;
        }
        m_isDrawing = false;
        update();
    }

}

void Ekran::setupUI()
{
    m_mainLayout = new QVBoxLayout(this);

    m_imgPanel = new QWidget(this);
    m_imgPanel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    m_imgLayout = new QVBoxLayout();
    m_imgLayout->addWidget(m_imgPanel);
    m_mainLayout->addLayout(m_imgLayout);

    m_spacer2 = new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Fixed);
    m_mainLayout->addItem(m_spacer2);

    m_buttonsLayout = new QHBoxLayout();

    m_resetButton = new QPushButton("Reset", this);
    m_textureButton = new QPushButton("Texture", this);
    m_interpolationCheckBox = new QCheckBox("Interpolation", this);

    m_buttonsLayout->addWidget(m_resetButton);
    m_buttonsLayout->addWidget(m_textureButton);
    m_buttonsLayout->addWidget(m_interpolationCheckBox);

    m_mainLayout->addLayout(m_buttonsLayout);

    setLayout(m_mainLayout);

    connect(m_resetButton, &QPushButton::clicked, this, &Ekran::reset);
    connect(m_textureButton, &QPushButton::clicked, this, [this]()
            {
                drawTriangle(m_canvasPoints, m_texturePoints, m_mode);
            });

    connect(m_interpolationCheckBox, &QCheckBox::toggled, this, [this](bool checked)
            {
                m_mode = checked ? InterpolationMode::ON : InterpolationMode::OFF;
            });
}


void Ekran::drawPixel(QImage &img, int x, int y, int h_color)
{
    if (x < 0 || x >= img.width() + m_texture.width() || y < 0 || y >= img.height()) return;

    uchar* line = img.scanLine(y);
    line[4*x] = h_color & 0xFF; //blue
    line[4*x + 1] = (h_color >> 8) & 0xFF; //green
    line[4*x + 2] = (h_color >> 16) & 0xFF; //red
    line[4*x + 3] = 255; // alpha
}

void Ekran::drawLineBresenham(QImage &img, const QPoint &first, const QPoint &second)
{
    int x1 = first.x();
    int x2 = second.x();
    int y1 = first.y();
    int y2 = second.y();

    if (std::abs(y2 - y1) == 0) //pozioma
    {
        if (x2 < x1)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for (int x = x1; x <= x2; x++)
        {
            drawPixel(img, x, y1, 0xFF0000);
        }
    }
    else if (std::abs(x2 - x1) == 0) //pionowa
    {
        if (y2 < y1)
        {
            std::swap(x1, x2);
            std::swap(y1, y2);
        }

        for (int y = y1; y <= y2; y++)
        {
            drawPixel(img, x1, y, 0xFF0000);
        }
    }
    else if (std::abs(y2 - y1) <= std::abs(x2 - x1))
    {
        if (x1 > x2)
        {
            std::swap(x1,x2);
            std::swap(y1, y2);
        }

        int dx = x2 - x1;
        int dy = y2 - y1;

        int m = (dy < 0) ? -1 : 1;
        dy *= m;
        int y = y1;
        int D = 2 * dy - dx;

        for (int x = x1; x <= x2; x++)
        {
            drawPixel(img, x, y, 0xFF0000);
            if (D > 0)
            {
                y += m;
                D -= 2 * dx;
            }
            D += 2 * dy;
        }
    }
    else
    {
        if (y1 > y2)
        {
            std::swap(x1,x2);
            std::swap(y1, y2);
        }

        int dx = x2 - x1;
        int dy = y2 - y1;

        int m = (dx < 0) ? -1 : 1;
        dx *= m;

        int x = x1;
        int D = 2 * dx - dy;

        for (int y = y1; y <= y2; y++)
        {
            drawPixel(img, x, y, 0xFF0000);
            if (D > 0)
            {
                x += m;
                D -= 2 * dy;
            }
            D += 2 * dx;
        }
    }

    update();
}

void Ekran::drawTriangle(const std::array<QPoint, 3> &points, const std::array<QPoint, 3>& texturePoints, InterpolationMode mode)
{


    int x_a = points[0].x();
    int y_a = points[0].y();

    int x_b = points[1].x();
    int y_b = points[1].y();

    int x_c = points[2].x();
    int y_c = points[2].y();

    int x_a_t = texturePoints[0].x();
    int y_a_t = texturePoints[0].y();
    int x_b_t = texturePoints[1].x();
    int y_b_t = texturePoints[1].y();
    int x_c_t = texturePoints[2].x();
    int y_c_t = texturePoints[2].y();

    int W = ((x_b - x_a) * (y_c - y_a)) - ((y_b - y_a) * (x_c - x_a));

    int min_y = std::min({y_a, y_b, y_c});
    int max_y = std::max({y_a, y_b, y_c});
    int min_x = std::min({x_a, x_b, x_c});
    int max_x = std::max({x_a, x_b, x_c});

    PixelColor textureColor;

    for (int y = min_y; y <= max_y; y++)
    {
        for (int x = min_x; x <= max_x; x++)
        {
            int W_v = ((x - x_a) * (y_c - y_a)) - ((x_c - x_a) * (y - y_a));
            int W_w = ((x_b - x_a) * (y - y_a)) - ((x - x_a) * (y_b - y_a));

            float v = W_v / (W * 1.0f);
            float w = W_w / (W * 1.0f);
            float u = 1 - v - w;

            if ((u >= 0 && v >= 0 && w >= 0) && (u <= 1 && v <= 1 && w <= 1))
            {

                float x_t = u * x_a_t + v * x_b_t + w * x_c_t;
                float y_t = u * y_a_t + v * y_b_t + w * y_c_t;

                if (mode == InterpolationMode::ON)
                {
                    PixelColor P1 = getPixel(m_temp, std::floor(x_t),
                                                        std::ceil(y_t));
                    PixelColor P2 = getPixel(m_temp, std::ceil(x_t),
                                                        std::ceil(y_t));
                    PixelColor P3 = getPixel(m_temp, std::ceil(x_t),
                                                        std::floor(y_t));
                    PixelColor P4 = getPixel(m_temp, std::floor(x_t),
                                                        std::floor(y_t));

                    float x_tt = x_t - std::floor(x_t);
                    float y_tt = y_t - std::floor(y_t);

                    textureColor.R = y_tt * ((1 - x_tt) * P1.R + x_tt * P2.R) + (1 - y_tt) * ((1 - x_tt) * P4.R + x_tt * P3.R);
                    textureColor.G = y_tt * ((1 - x_tt) * P1.G + x_tt * P2.G) + (1 - y_tt) * ((1 - x_tt) * P4.G + x_tt * P3.G);
                    textureColor.B = y_tt * ((1 - x_tt) * P1.B + x_tt * P2.B) + (1 - y_tt) * ((1 - x_tt) * P4.B + x_tt * P3.B);
                }
                else
                {
                    textureColor = getPixel(m_temp, x_t, y_t);
                }

                setPixel(m_canvas, x, y, textureColor);
            }
        }
    }
    update();
}

PixelColor Ekran::getPixel(const QImage &img, int x, int y) const
{
    const uchar* line = img.scanLine(y);
    uint8_t blue = line[4*x]; //blue
    uint8_t green = line[4*x + 1]; //green
    uint8_t red = line[4*x + 2]; //red
    //uint8_t alpha = line[4*x + 3]; // alpha


    return PixelColor{red, green, blue};
}

void Ekran::setPixel(QImage &img, int x, int y, const PixelColor &color)
{
    int red = color.R;
    int green = color.G;
    int blue = color.B;

    uchar* line = img.scanLine(y);
    line[4*x] = blue; //blue
    line[4*x + 1] = green; //green
    line[4*x + 2] = red; //red
    //line[4*x + 3] = 255; // alpha
}

void Ekran::drawCircle(QImage &img, const QPoint &first, float r)
{
    int x1 = first.x();
    int y1 = first.y();

    float L = std::round(r/std::sqrt(2));
    float y;

    for (int i = 0; i <= L; i++)
    {
        y = std::sqrt(r * r - i * i);

        // Pierwsza cwiartka
        drawPixel(img, i + x1, std::round(y) + y1, 0xFF0000);
        drawPixel(img, std::round(y) + x1, i + y1, 0xFF0000);

        // Druga cwiartka
        drawPixel(img, std::round(y) + x1, -i + y1 , 0xFF0000);
        drawPixel(img, i + x1, -std::round(y) + y1 , 0xFF0000);

        // Trzecia cwiartka
        drawPixel(img, -i + x1, -std::round(y) + y1 , 0xFF0000);
        drawPixel(img, -std::round(y) + x1, -i + y1 , 0xFF0000);

        // Czwarta cwiartka
        drawPixel(img, -i + x1, std::round(y) + y1 , 0xFF0000);
        drawPixel(img, -std::round(y) + x1, i + y1 , 0xFF0000);

    }

    for (int i = -std::ceil(r); i <= std::ceil(r); i++)
    {
        for (int j = -std::ceil(r); j <= std::ceil(r); j++)
        {
            if (std::sqrt(j * j + i * i) <= r) {
                drawPixel(img, x1 + j, y1 + i, 0xFF00FF);
            }
        }
    }


    update();
}


void Ekran::reset()
{
    m_texture = m_temp.copy();
    m_canvas.fill(0);

    update();
}

void Ekran::setInterpolationMode()
{
    m_mode = InterpolationMode::ON;
}
