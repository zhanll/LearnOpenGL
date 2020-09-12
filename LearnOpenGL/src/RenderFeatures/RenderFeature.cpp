#include "RenderFeature.h"
#include "../Camera.h"
#include "../Config.h"
#include <glm/gtc/matrix_transform.hpp>

void RenderFeatureBase::Render()
{
    m_MatView = m_Camera->GetViewMatrix();
    m_MatProjection = glm::perspective(glm::radians(m_Camera->GetFOV()), (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT, 0.1f, 100.0f);
}