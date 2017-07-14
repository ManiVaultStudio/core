#ifndef OPENGL_HELPERS_H
#define OPENGL_HELPERS_H
namespace nut{

	class ScopedCapabilityEnabler{
	public:
		ScopedCapabilityEnabler(GLenum capability):
			_capability(capability)
		{
			glEnable(_capability);
		}
		~ScopedCapabilityEnabler(){
			glDisable(_capability);
		}
	private:
		GLenum _capability;
	};

	class ScopedCapabilityDisabler{
	public:
		ScopedCapabilityDisabler(GLenum capability):
			_capability(capability)
		{
			glDisable(_capability);
		}
		~ScopedCapabilityDisabler(){
			glEnable(_capability);
		}
	private:
		GLenum _capability;
	};

}

#endif