package bartek.weather.information;

public class Air {
	private double pressure;
	private double cloudiness;
	private double humidity;	
	private double temperature;
	private Wind wind;
	
	private boolean isKnownPressure;
	private boolean isKnownCloudiness;
	private boolean isKnownHumidity;
	private boolean isKnownTemperature;
	private boolean isKnownWind;
	
	public Air() {
		super();
		this.isKnownPressure = false;
		this.isKnownCloudiness = false;
		this.isKnownHumidity = false;
		this.isKnownTemperature = false;
		this.isKnownWind = false;
	}
	public void setPressure(double pressure) {
		this.pressure = pressure;
		this.isKnownPressure = true;
	}
	public void setCloudiness(double cloudiness) {
		this.cloudiness = cloudiness;
		this.isKnownCloudiness = true;
	}
	public void setHumidity(double humidity) {
		this.humidity = humidity;
		this.isKnownHumidity = true;
	}
	public void setTemperature(double temperature) {
		this.temperature = temperature;
		this.isKnownTemperature = true;
	}
	public void setWind(Wind wind) {
		this.wind = wind;
		this.isKnownWind = true;
	}
	
	public double getPressure() throws Exception {
		if(isKnownPressure)
			return pressure;
		else
			throw new Exception();
	}
	public double getCloudiness() throws Exception{
		if(isKnownCloudiness)
			return cloudiness;
		else
			throw new Exception();
	}
	public double getHumidity() throws Exception{
		if(isKnownHumidity)
			return humidity;
		else
			throw new Exception();
	}
	public double getTemperature() throws Exception{
		if(isKnownTemperature)
			return temperature;
		else
			throw new Exception();
	}
	public Wind getWind() throws Exception{
		if(isKnownWind)
			return wind;
		else
			throw new Exception();
	}
	
}
