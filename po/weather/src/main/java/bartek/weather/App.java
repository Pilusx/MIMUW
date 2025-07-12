package bartek.weather;

import java.net.URL;
import bartek.weather.gui.Controller;
import bartek.weather.information.*;
import io.reactivex.Observable;
import io.reactivex.rxjavafx.observables.JavaFxObservable;
import javafx.stage.Stage;
import javafx.application.Application;
import javafx.application.Platform;
import javafx.event.ActionEvent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ComboBox;
import javafx.scene.layout.Pane;
import javafx.fxml.FXMLLoader;

public class App extends Application
{	
	private static Place place = new Place("Warsaw", "Poland", "Warszawa", 
			"http://api.openweathermap.org/data/2.5/weather?q=Warsaw,PL&APPID=96d37e3bd9c70a4a3b6f9f1626823cd2",
			"http://www.meteo.waw.pl/");	
	
	private static long timerDelay = 5*60*1000; // 5 minutes
	private static String guiFXMLPath = "gui/gui.fxml";
	private Button refreshButton;
	private ComboBox<WeatherSource> airStations;
	private WeatherSource weatherSource;
	
	@Override
	public void start(Stage primaryStage) throws Exception {
		URL location = this.getClass().getResource(guiFXMLPath);
		FXMLLoader fxmlLoader = new FXMLLoader(location);
		
		Pane root = (Pane) fxmlLoader.load();
		Controller controller = fxmlLoader.getController();
		controller.initAirStations();
		
		refreshButton = controller.getRefreshButton();
		airStations = controller.getAirStations();
		weatherSource = airStations.getValue();
		
		Observable<ActionEvent> refreshing = JavaFxObservable.actionEventsOf(refreshButton);
		refreshing.subscribe(e -> {
			controller.refreshWeather(weatherSource, place);
			System.out.println("[DEBUG] Refreshed weather: " + weatherSource);
		});
		
		
		Observable<ActionEvent> sourceSelection = JavaFxObservable.actionEventsOf(airStations);
		sourceSelection.subscribe(e -> {
			weatherSource = airStations.getValue();
			System.out.println("[DEBUG] Weather Source changed to " + weatherSource);
		});
		
		Observable<Long> emitter = JavaFxObservable.interval(javafx.util.Duration.millis(timerDelay));
		emitter.subscribe(e -> {
			System.out.println("[DEBUG] Scheduled refresh");
			refreshButton.fire();
		});
		
		refreshButton.fire();
	
		primaryStage.setScene(new Scene(root));
		primaryStage.show();
		primaryStage.setTitle("Weather");
	}
	
	public static void main (String[] args) {
		Platform.setImplicitExit(true);
		Application.launch(App.class, args);
	}
}
