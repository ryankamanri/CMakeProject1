#pragma once
// #include "triangle3d.hpp"


namespace Kamanri
{
	namespace Renderer
	{
		namespace World
		{
			namespace __
			{
				class Configs
				{
					public:
					bool is_commited = false;
					bool is_use_cuda = false;
					Configs& operator=(Configs&& other)
					{
						is_commited = other.is_commited;
						is_use_cuda = other.is_use_cuda;
						return *this;
					}

				};
                

			} // namespace __

		} // namespace World

	} // namespace Renderer

} // namespace Kamanri
