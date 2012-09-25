#pragma once

namespace Skynet {
	public interface class SkynetControllerInterface {
		void saveCurrentFrameAsCandidate();
		void intendedGimbalPositionUpdated( float rollDegrees, float pitchDegrees );
		void intendedCameraZoomUpdated( float zoom );
		void stopTargetLock();
	};
}