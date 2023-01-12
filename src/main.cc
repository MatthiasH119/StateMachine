#include <iostream>
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

int main (int argc, char** argv){

    std::cout << "Hello there!" << std::endl;
    CLI::App app{"Dies ist ein Programm, mit dem der Umgang mit CMake, CLI11 und der nlohmann/json lib getestet werden soll","Testprogramm"};

/*  
    #   Einführung der option r (read), eingabe wird als string in 'filepath' geschrieben
    #   "->required" wird als erste Flag bei dieser Notation immer benötigt; wird beim Programmaufruf nicht -r <filepath> eingegeben, gibt das Programm einen entsprechenden Fehler aus.
    #   "->check(...)" überprüft, ob das File existiert
*/
    std::string filepath;
    app.add_option("-r,--read", filepath, "Path to config file")
        ->required()
        ->check(CLI::ExistingFile);
        
    
    bool inhalt_flag = false, change_flag, count_flag;
    app.add_option("-i, --inhalt", inhalt_flag, "Gibt Inhalt des Lagers aus. Dazu '-i 1' oder '-i true' eingeben.");

    app.add_option("-c, --change", change_flag, "Ändert den Inhalt des Lagers. Dazu '-c 1' oder '-c true' eingeben.");

    app.add_option("-z, --count", count_flag, "Zählt den Inhalt des Lagers. Dazu '-z 1' oder '-z true' eingeben.");
  
//  #   mit der try-catch Strategie wird hier die argc/argv Mechanik ausgeführt. Ohne diese stehen die "app.add_option" zeilen ohne Funktion im Programm
    try{
        app.parse(argc, argv);
    } catch(const CLI::ParseError& e){
        return app.exit(e);
    }

//  #   Hier wird das File geöffnet und anschließend überprüft, ob es richtig geöffnet ist. Wenn nicht, wird das Programm mit dem exit Wert 0 beendet.
    std::ifstream file{filepath};
    if(!file.is_open()){
        std::cout << "Error opening File!" << std::endl;
        exit(0);
    }

    
//  #   Anlegen der Variable 'database_object' mit Datentyp 'json' aus der nlohmann Lib
    nlohmann::json database_object;

//  #   try-catch Methode zum Analysieren der geöffneten Datei
    try
    {
        database_object = nlohmann::json::parse(file);
    }
    catch (nlohmann::json::parse_error& ex)
    {
        std::cerr << "parse error at byte" << ex.byte << std::endl;
    }

//  #   Im Programmaufruf kann mit '-i true' eine Flag gesetzt werden. Diese bewirkt im folgenden, dass der Inhalt der Regale ausgegeben wird.
    if(inhalt_flag == true){
        for (auto& element : database_object["Regale"]){
            std::cout << "\nRegal Nr.: "        << element["Regal"]                 << std::endl;
            std::cout << "Anzahl Lagerplätze: " << element["Anzahl Lagerplätze"]    << std::endl;
            std::cout << "Inhalt des Regals: "  << element["Inhalt"]                << std::endl;
        }
    }

//  #   Im Programmaufruf kann mit '-z true' eine Flag gesetzt werden. Diese bewirkt im folgenden, dass der Inhalt der Regale gezählt werden kann.
    if(count_flag == true){
        for (auto& member : database_object["Regale"]){
            int item_counter = 0;
            for (auto& Inhalt : member["Inhalt"]){
                item_counter++;
            }
            std::cout << "Anzahl der Elemente: " << item_counter << std::endl;
        }
    }

//  #   Im Programmaufruf kann mit '-c true' eine Flag gesetzt werden. Diese bewirkt im folgenden, dass der Inhalt der Regale geändert werden kann.
    if(change_flag == true){
        std::cout << std::endl << "MENÜPUNKT: ÄNDERUNGEN" << std::endl;             //Dialog
        int counter_change_flag = 0;                                                //Einführen einer Variablen zum erfassen, ob und wie viele Änderungen vorgenommen werden.

        for (auto& change : database_object["Regale"]){                             //Hier werden die Änderungen vorgenommen. Vorher wird abgefragt, in welchem Regal geändert werden soll.
            std::cout << "Inhalt von Regal Nr. " << change["Regal"] << ": " << change["Inhalt"] << std::endl;

            char auswahl = '0';
            std::cout << "Möchten Sie diesen Regalinhalt ändern? Ja (1) - Nein (0)" << std::endl << "Ihre Auswahl: ";
            std::cin  >> auswahl;
                
            if(auswahl == '1'){
                std::cin  >> change["Inhalt"];
                std::cout << "Geänderter Inhalt: " << change["Inhalt"] << std::endl;
                counter_change_flag++;                   
            }
        }
        if(counter_change_flag != 0){                                               //Wenn Änderungen vorgenommen werden, wird hier abgefragt, ob und wo diese gespeichert werden sollen.
            std::cout << "Anzahl der Änderungen: " << counter_change_flag << std::endl;

            counter_change_flag = 0;                                                //Reset der Variable, diese wird nun für eine andere Abfrage verwendet.
            std::cout << std::endl << "Möchten Sie die Änderungen speichern? Ja (1) - Nein (0)" << std::endl << "Ihre Auswahl: ";
            std::cin  >> counter_change_flag;

            if(counter_change_flag == 1){
                std::string datei_ausgabe{};
                std::cout << "Name der Ausgabedatei: ";
                std::cin >> datei_ausgabe;

                std::ofstream save_as{datei_ausgabe};
                save_as << database_object.dump(4);
                save_as.close();
            }
            else {
                std::cout << "Änderungen wurden verworfen!" << std::endl;
            }
        }
    }

    return 0;   
}