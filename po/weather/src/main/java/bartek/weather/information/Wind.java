package bartek.weather.information;

public class Wind {
	private double angle;
	private double speed;
	private boolean isKnownAngle;
	private boolean isKnownSpeed;
	
	public Wind() {
		super();
		this.isKnownAngle = false;
		this.isKnownSpeed = false;
	}
	public void setAngle(double angle) {
		this.angle = angle;
		this.isKnownAngle = true;
	}
	public void setSpeed(double speed) {
		this.speed = speed;
		this.isKnownSpeed = true;
	}
	
	public double getAngle() throws Exception{
		if(!isKnownAngle)
			throw new Exception();
		else 
			return angle;
	}
	
	public double getSpeed() throws Exception{
		if(!isKnownSpeed)
			throw new Exception();
		else 
			return speed;
	}
}
