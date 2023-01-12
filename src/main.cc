#include <iostream>
#include <CLI/CLI.hpp>
#include <nlohmann/json.hpp>

struct statemachine_t {
    statemachine_t(nlohmann::json& database_object)
            :mein_json{database_object} {}
            
    private:
        nlohmann::json& mein_json;

        void state_exit(){
            std::cout << "OH! Looks like something went wrong!" << std::endl;
            std::exit(EXIT_FAILURE);
        }

        //  #   state_safe_exit -> richtiges beenden des Programms mit Speicher abfrage
        void state_safe_exit(){
            //Funktion zum Speichern () ?!
            std::cout << "Das Programm wird beendet!" << std::endl;
            std::exit(EXIT_SUCCESS);
        }

        //  #   state_inhalt -> gibt den Inhalt der json aus
        void state_inhalt(){
            for (auto& element : mein_json["Regale"]){
                std::cout << "\nRegal Nr.: "        << element["Regal"]                 << std::endl;
                std::cout << "Anzahl Lagerplätze: " << element["Anzahl Lagerplätze"]    << std::endl;
                std::cout << "Inhalt des Regals: "  << element["Inhalt"]                << std::endl;
            }
        }

        //  #   state_count -> zählt den Inhalt der Regale
        void state_count(){
            for (auto& member : mein_json["Regale"]){
                int item_counter = 0;
                for (auto& Inhalt : member["Inhalt"]){
                    item_counter++;
                }
                std::cout << "Anzahl der Elemente: " << item_counter << std::endl;
            }
        }

        //  #   state_change -> zum ändern von Inhalt
        /*
        void state_change(){
            std::cout << std::endl << "MENÜPUNKT: ÄNDERUNGEN" << std::endl;             //Dialog
            int counter_change_flag = 0;                                                //Einführen einer Variablen zum erfassen, ob und wie viele Änderungen vorgenommen werden.

            for (auto& change : mein_json["Regale"]){                             //Hier werden die Änderungen vorgenommen. Vorher wird abgefragt, in welchem Regal geändert werden soll.
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
        */
       
       int print_menu_options(){
        int option = 0;
        while(true){
            option = 0;
            std::cout << "Was möchten Sie tun?: "   << std::endl
                    << "(1) Inhalt ausgeben"        << std::endl
                    << "(2) Inhalt zählen"          << std::endl
                    << "(3) Inhalt ändern"          << std::endl
                    << "(4) Programm beenden"       << std::endl;
            std::cin >> option;
            if (option > 4 || (option < 0)){
                std::cout << "Inkorrekte Eingabe!" << std::endl;
            }
            else {
                break;
            }
        }
        return option;
       }


    public:
        void run(){
            //print welcome dialog
            std::cout << "Hallöchen und Wilkommen im Lager." << std::endl;

            //superloop bzw. OB1 ^^
            while(true){
                int auswahl = print_menu_options();
                switch(auswahl){
                    case 1: 
                        state_inhalt();
                        break;
                    case 2: 
                        state_count();
                        break;
                    //case 3: state_change();
                    case 4: 
                        state_safe_exit();
                        break;
                    case 0:
                        state_exit();
                        break;
                }

            }
        }
};

int main (int argc, char** argv){

    std::cout << "Hello there!" << std::endl;
    CLI::App app{"Dies ist ein Programm, mit dem der Umgang mit State Machines, CMake, CLI11 und der nlohmann/json lib getestet werden soll","Testprogramm"};

    //String für Optionsschalter
    std::string filepath;

    app.add_option("-r,--read", filepath, "Path to config file")
        ->required()
        ->check(CLI::ExistingFile);
        
    try{
        app.parse(argc, argv);
    } catch(const CLI::ParseError& e){
        return app.exit(e);
    }

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

    statemachine_t statemachine_lagerhaltung{database_object};

    //starte main routine
    statemachine_lagerhaltung.run();

    return 0;   
}