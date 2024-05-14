#pragma once

#ifdef SPACEAPI_EXPORTS
#define SPACEAPI_API _declspec(dllexport)
#else 
#define SPACEAPI_API _declspec(dllimport)
#endif

#include <cstdlib>

enum class dirs { stop = 0, up = 1, down = 2, left = 3, right = 4, u_l = 5, u_r = 6, d_l = 7, d_r = 8 };
enum class types { no_type = 0, hero = 1, evil1 = 2, evil2 = 3, evil3 = 4 };
enum class choices { no_action = 0, move = 1,  shoot = 2 };


constexpr float scr_width = 1000.0f;
constexpr float scr_height = 750.0f;

constexpr int  DLL_OK = 15001;
constexpr int  DLL_FAIL = 15002;
constexpr int  DLL_UNKNOWN = 15003;

namespace space
{
	struct SPACEAPI_API LINEDATA
	{
		float slope = 0;
		float intercept = 0;
	};

	struct SPACEAPI_API AIDATA
	{
		float my_x = 0;
		float my_y = 0;
		float my_ex = 0;
		float my_ey = 0;
		
		float enemy_x = 0;
		float enemy_y = 0;
		float enemy_ex = 0;
		float enemy_ey = 0;
		
		float speed = 1.0f;
	
		bool enemy_in_range = false;

	};
	struct SPACEAPI_API AIOUTPUT
	{
		choices new_action = choices::no_action;
		float new_x = 0;
		float new_y = 0;
		dirs new_dir = dirs::stop;
	};

	/////////////////////////////////////////

	class SPACEAPI_API OBJECT
	{
		protected:
			float width = 0;
			float height = 0;

		public:
			float x = 0;
			float y = 0;
			float ex = 0;
			float ey = 0;

			OBJECT(float _x, float _y, float _width = 1.0f, float _height = 1.0f)
			{
				x = _x;
				y = _y;
				width = _width;
				height = _height;
				ex = x + width;
				ey = y + height;
			}
			~OBJECT() {};

			void ObjectRelease(OBJECT** obj)
			{
				delete (*obj);
			}

			float GetWidth() const
			{
				return width;
			}
			float GetHeight() const
			{
				return height;
			}
			void SetWidth(float _new_width)
			{
				width = _new_width;
				ex = x + width;
			}
			void SetHeight(float _new_height)
			{
				height = _new_height;
				ey = y + height;
			}
			void SetEdges()
			{
				ex = x + width;
				ey = y + height;
			}
			void NewDims(float _new_width, float _new_height)
			{
				width = _new_width;
				height = _new_height;
				ex = x + width;
				ey = y + height;
			}
	};

	class SPACEAPI_API FACTORY :public OBJECT
	{
		protected:
			float speed = 1.0f;
			LINEDATA LineData;
			int shoot_delay = 1;

		public:
			types type = types::no_type;
			dirs dir = dirs::stop;
			int lifes = 0;

			FACTORY(types _type, float _x, float _y) :OBJECT(_x, _y)
			{
				type = _type;
				switch (type)
				{
				case types::hero:
					speed = 1.2f;
					lifes = 150;
					NewDims(48.0f, 48.0f);
					shoot_delay = 50;
					break;

				case types::evil1:
					speed = 0.8f;
					lifes = 100;
					NewDims(40.0f, 39.0f);
					shoot_delay = 80;
					break;

				case types::evil2:
					speed = 0.5f;
					lifes = 150;
					NewDims(50.0f, 48.0f);
					shoot_delay = 120;
					break;

				case types::evil3:
					speed = 0.9f;
					lifes = 120;
					NewDims(45.0f, 45.0f);
					shoot_delay = 100;
					break;
				}
			}

			virtual ~FACTORY() {};

			LINEDATA InitLine(float start_x, float start_y, float end_x, float end_y)
			{
				if (abs(end_x - start_x) == 0)
				{
					LineData.intercept = 0;
					if (end_y > start_y)LineData.slope = 1;
					else LineData.slope = -1;
				}
				else
				{
					LineData.slope = (end_y - start_y) / (end_x - start_x);
					LineData.intercept = start_y - (LineData.slope * start_x);
				}
				return LineData;
			}
			float NextY(float _next_x) const
			{
				return LineData.slope * _next_x + LineData.intercept;
			}
			virtual void Release() = 0;
			virtual int Move(float _speed) = 0;
			virtual bool Shoot() = 0;
	};

	/////////////////////////////////////////

	typedef FACTORY* Person;

	extern SPACEAPI_API LINEDATA InitLineData(float start_x, float start_y, float end_x, float end_y);

	extern SPACEAPI_API float NextLineY(float _next_x,LINEDATA &Data);

	extern SPACEAPI_API Person iCreatePerson(types who, float where_x, float where_y);

	///////////////////////////////////////////

	extern SPACEAPI_API AIOUTPUT AIToDo(AIDATA CurrentAction);
}