#include "Texture.h"
#include <boost/algorithm/clamp.hpp>
using namespace boost::gil;
using boost::algorithm::clamp;

Texture::Texture(const std::string& texture_path)
{
	m_img_loaded = false;
	setTexturePath(texture_path);
	loadTexture();
}


Texture::~Texture(void)
{
}

void 
Texture::loadTexture()
{
	if (m_texture_path.empty())
	{
		m_img_loaded = false;
		return;
	}
	try
	{
	    boost::gil::jpeg_read_image(m_texture_path, m_img);
		printf("GIL: ('%s') texture map loaded\n", m_texture_path.c_str());
		m_img_loaded = true;
	}
	catch (std::exception &e)
	{
		printf("GIL: ('%s') %s\n", m_texture_path.c_str(), e.what());
		m_img_loaded = false;
	}
}

void
Texture::setTexturePath(const std::string& texture_path)
{
	m_texture_path = texture_path;
	m_texture_path.erase(std::remove(m_texture_path.begin(), m_texture_path.end(), '\n'), m_texture_path.end());
	m_texture_path.erase(std::remove(m_texture_path.begin(), m_texture_path.end(), '\r'), m_texture_path.end());
	loadTexture();
}

Vector3
Texture::shade(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	if (m_img_loaded)
	{
		boost::gil::rgb8_pixel_t pixel = m_img._view((int) (fmodf(std::abs(hit.u), 1) * m_img.width()), (int) (fmodf(std::abs(hit.v), 1) * m_img.height()));
		return Vector3(pixel[0] / 255.f, pixel[1] / 255.f, pixel[2] / 255.f);
	}
	else
		return m_color;
}

Vector3
Texture::computeGradient(const Ray& ray, const HitInfo& hit, const Scene& scene) const
{
	if (m_img_loaded)
	{
		int x = (int) (fmodf(std::abs(hit.u), 1) * m_img.width());
		int y = (int) (fmodf(std::abs(hit.v), 1) * m_img.height());

		int x_minus = x > 0 ? x - 1 : m_img.width() - 1;
		int y_minus = y > 0 ? y - 1 : m_img.height() - 1;
		int x_plus = x + 1 < m_img.width() ? x + 1 : 0;
		int y_plus = y + 1 < m_img.height() ? y + 1 : 0;


		boost::gil::rgb8_pixel_t pixel1 = m_img._view(x_minus, y);
		boost::gil::rgb8_pixel_t pixel2 = m_img._view(x_plus, y);
		boost::gil::rgb8_pixel_t pixel3 = m_img._view(x, y_minus);
		boost::gil::rgb8_pixel_t pixel4 = m_img._view(x, y_plus);
		return Vector3(pixel1[0] - pixel2[0], pixel3[0] - pixel4[0], 0);
	}
	else
		return m_color;
}