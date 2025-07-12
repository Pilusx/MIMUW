package bartek.weather.information;

public class Place {
	private String city;
	private String country;
	private String powietrzeGiosCityName;
	private String openWeatherLink;
	private String meteoLink;
	public Place(String city, String country, String powietrzeGiosCityName, String openWeatherLink, String meteoLink) {
		super();
		this.city = city;
		this.country = country;
		this.powietrzeGiosCityName = powietrzeGiosCityName;
		this.openWeatherLink = openWeatherLink;
		this.meteoLink = meteoLink;
	}
	public String getCity() {
		return city;
	}
	public String getCountry() {
		return country;
	}
	public String getOpenWeatherLink() {
		return openWeatherLink;
	}
	public String getPowietrzeGiosCityName() {
		return powietrzeGiosCityName;
	}
	public String getMeteoLink() {
		return meteoLink;
	}
}
