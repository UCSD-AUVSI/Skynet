namespace Vision {
	public ref struct TargetResult
	{
		System::String ^ letter;
		System::String ^ shape;
		System::String ^ letterColor;
		System::String ^ shapeColor;
		double rotationDegrees;

		/**
		 * Orientation of the target, counterclockwise from due east.
		 */
		double targetOrientationDegrees;

		TargetResult(System::String ^ letter, System::String ^ shape, System::String^ letterColor, System::String^ shapeColor, double targetOrientationDegrees): 
			letter(letter), shape(shape), letterColor(letterColor), shapeColor(shapeColor), targetOrientationDegrees(targetOrientationDegrees) {}

		System::String^ getHeadingString(){
			if (targetOrientationDegrees > 0 && targetOrientationDegrees < 22.5){
				return "E";
			} else if (targetOrientationDegrees < 67.5){
				return "NE";
			} else if (targetOrientationDegrees < 112.5){
				return "N";
			} else if (targetOrientationDegrees < 157.5){
				return "NW";
			} else if (targetOrientationDegrees < 202.5){
				return "W";
			} else if (targetOrientationDegrees < 247.5){
				return "SW";
			} else if (targetOrientationDegrees < 292.5){
				return "S";
			} else if (targetOrientationDegrees < 337.5){
				return "SE";
			} else if (targetOrientationDegrees < 360){
				return "E";
			} else {
				throw gcnew Exception("targetOrientation must be at least 0 and less than 360");
			}
		}
	};
}