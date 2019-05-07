/* Primal.h -- interface of the 'Primal Engine'
  Copyright (C) 2019 Roderick Griffioen
  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.
  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:
  1. The origin of this software must not be misrepresented; you must not
	 claim that you wrote the original software. If you use this software
	 in a product, an acknowledgment in the product documentation would be
	 appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
	 misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/
#ifndef primal_h__
#define primal_h__

// API includes
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Application
#include "application/Application.h"
#include "application/ApplicationLayer.h"

// Assets
#include "assets/Asset.h"
#include "assets/AssetManager.h"

// Components
#include "components/MeshContainerComponent.h"
#include "components/MeshRenderComponent.h"
#include "components/TransformComponent.h"

// Core
#include "core/Log.h"
#include "core/PrimalAssert.h"
#include "core/Timer.h"
#include "core/Window.h"

// ECS
#include "ecs/Component.h"
#include "ecs/ComponentView.h"
#include "ecs/Entity.h"
#include "ecs/EntityManager.h"
#include "ecs/System.h"
#include "ecs/SystemManager.h"

// Events
#include "events/Event.h"
#include "events/ApplicationEvent.h"
#include "events/KeyEvents.h"

// Input
#include "input/Input.h"
#include "input/KeyCodes.h"

// File System
#include "filesystem/FileSystem.h"
#include "filesystem/File.h"

// Graphics
#include "graphics/Mesh.h"
#include "graphics/VertexArray.h"
#include "graphics/VertexBuffer.h"
#include "graphics/VertexBufferLayout.h"
#include "graphics/IndexBuffer.h"

// Systems
#include "systems/RenderSystem.h"

#endif // primal_h__