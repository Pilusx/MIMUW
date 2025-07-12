package bartek.weather.gui;

import java.time.LocalTime;
import java.time.format.DateTimeFormatter;

import bartek.weather.information.Air;
import bartek.weather.information.ParticulateMatter;
import bartek.weather.information.Place;
import bartek.weather.information.Weather;
import bartek.weather.information.WeatherSource;
import bartek.weather.information.Wind;
import javafx.fxml.FXML;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.control.Label;
import javafx.scene.layout.AnchorPane;
import javafx.scene.layout.VBox;

public class Controller extends AnchorPane{
	@FXML private VBox vbox;
	@FXML private Label placeLabel;
	@FXML private Label temperatureLabel;
	@FXML private Label cloudinessLabel;
	@FXML private Label pm25Label;
	@FXML private Label pm10Label;
	@FXML private Label pressureLabel;
	@FXML private Label humidityLabel;
	@FXML private Label lastUpdateLabel;
	@FXML private Label windAngleLabel;
	@FXML private Label windSpeedLabel;
	@FXML private Button refreshButton;
	@FXML private ComboBox<WeatherSource> airStations;

	public void refreshWeather(WeatherSource weatherSource, Place place){
		Weather w = new Weather(weatherSource, place);
		
		setAir(w.getAir());
		setParticulateMatter(w.getParticulateMatter());
		
		placeLabel.setText(place.getCity() + ", " + place.getCountry());

		LocalTime time = w.getTime();
		lastUpdateLabel.setText("Updated at " + 
			time.format(DateTimeFormatter.ofPattern("K:mm a")));
			
	}
	
	public void initAirStations(){
		airStations.getItems().addAll(WeatherSource.Meteo, WeatherSource.OpenWeather);
		airStations.getSelectionModel().select(0);
	}
	
	private void setAir(Air air){
		String temperatureText = "";
		String cloudinessText = "Cloudiness: ";
		String pressureText = "Pressure: ";
		String humidityText = "Humidity: ";
		String windSpeedText = "Wind speed: ";
		String windAngleText = "Wind direction: ";
		
		try {
			temperatureText += air.getTemperature() + "°C";
		} catch (Exception e) {
			temperatureText = "- C";
		}
		
		try {
			cloudinessText += air.getCloudiness() + "%";
		} catch (Exception e) {
			cloudinessText += "-";
		}
		
		try {
			pressureText += air.getPressure() + "hPa";
		} catch (Exception e) {
			pressureText += "-";
		}
		try {
			humidityText += air.getHumidity() + "%";
		} catch (Exception e) {
			humidityText += "-";
		}
		
		try {
			Wind wind = air.getWind();
			try {
				windSpeedText += wind.getSpeed() + "m/s";
			} catch (Exception e){
				windSpeedText += "-";
			}
			try {
				String direction = convertWindAngleToDirection(wind.getAngle());
				windAngleText += direction;
			} catch (Exception e){
				windAngleText += "-";
			}
		} catch (Exception e) {
			windSpeedText += "-";
			windAngleText += "-";
		}
		
		temperatureLabel.setText(temperatureText);
		cloudinessLabel.setText(cloudinessText);
		pressureLabel.setText(pressureText);
		humidityLabel.setText(humidityText);
		windAngleLabel.setText(windAngleText);
		windSpeedLabel.setText(windSpeedText);
	}
	
	private String convertWindAngleToDirection(double d){
		String dir[] = {"N","NNE","NE","ENE","E","ESE","SE","SSE","S","SSW","SW","WSW","W","WNW","NW","NNW","N"};
		Double d2 = d/22.5;
		return dir[d2.intValue()];
	}
	
	private void setParticulateMatter(ParticulateMatter pm){
		String pm25Text = "PM2.5: ";
		String pm10Text = "PM10: ";
		
		try {
			pm25Text += pm.getPM25() + "%";
		} catch (Exception e){
			pm25Text += "-";
		}
		
		try {
			pm10Text += pm.getPM10() + "%";
		} catch (Exception e){
			pm10Text += "-";
		}

		pm25Label.setText(pm25Text);
		pm10Label.setText(pm10Text);
	}
	

	public Button getRefreshButton() {
		return refreshButton;
	}

	public ComboBox<WeatherSource> getAirStations() {
		return airStations;
	}
}
