/*
 * Copyright (c) 2022 Ronald Leenes
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *



 * @file graph.ino
 * @author Ronald Leenes

 *
 * @brief This file contains the graphing functions
 *
 * @see http://esp8266thingies.nl
 */

#if GRAPH == 1

#include "./timeutils.h"
#include "./statistics/Statistics.h"
#include "./statistics/StatisticsLine.h"
#include "./statistics/StatisticsStream.h"

void handleGraphMenu() {
  String str = "";

  addHead(str, 0);
  addIntro(str);

  str += F("<fieldset><legend><b> Grafieken </b></legend>");
  str += F("<fieldset><legend><b> periode </b></legend>");
  str += F("<form action='/SelectGraph' method='POST'>");
  str += F("<div ><table width='95%' ><tr>");
  str += F("<td width='25%'><input type='radio' id='day' name='period' "
           "value='day' checked /><label for='day'>vandaag</label></td>");
  str += F("<td width='25%'><input type='radio' id='week' name='period' "
           "value='week' /><label for='week'>deze week</label></td>");
  str += F("<td width='25%'><input type='radio' id='month' name='period' "
           "value='month' /><label for='month'>deze maand</label></td>");
  str += F("<td width='25%'><input type='radio' id='year' name='period' "
           "value='year' /><label for='year'>dit "
           "jaar</label></td></tr></table></br>");
  str += F("</div>");

  //  str += F("<fieldset>");
  str += F("<button name='graphtype' value='1' type='submit'>Energieverbruik "
           "T1/T2</button>");
  str += F("<p><button name='graphtype' value='2' type='submit'>Energie retour "
           "T1/T2</button>");
  str += F("<p><button name='graphtype' value='3' type='submit'>Energie "
           "totaal/retour</button>");
  str += F("<p><button name='graphtype' value='4' "
           "type='submit'>Gasverbruik</button>");
  str += F("</form>");
  str += F("</fieldset>");

  str += F("<fieldset><legend><b> Jaaroverzicht</b></legend>");
  str += F("<form action='/GasCalendar' method='POST'>");
  str += F("<p><button type='submit'>Gasverbruik dit jaar</button></form>");
  str += F("</fieldset></form>");

  str += F("</fieldset>"); // grafieken
  //  str += F("<form action='/Dir' method='GET'><button
  //  type='submit'>Directory</button></form>");
  str += F("<form action='/' method='POST'><button class='button "
           "bhome'>Menu</button>");

  addFootBare(str);
  server.send(200, "text/html", str);
}

void DumpDataFiles() {
  listDir("/");
  Log.verboseln("");
}

void selectGraph() {
  char switcher[3];
  strncpy(switcher, server.arg("graphtype").c_str(),
          server.arg("graphtype").length());
  switcher[1] = '\0';

  char period[6];
  strncpy(period, server.arg("period").c_str(), server.arg("period").length());
  period[server.arg("period").length()] = '\0';

  Log.verbose("Selected graph: ");
  Log.verboseln(switcher);
  Log.verbose("Selected period: ");
  Log.verboseln(period);

  if (server.method() == HTTP_POST) {
    //  int switcher = atoi(server.arg("datatype").c_str());
    Log.verboseln(switcher);
    switch (atoi(switcher)) {
    case 1: // delivered T1/T2
      theGraph("E1", "E2", "Elektriciteit gebruik T1",
               "Elektriciteit gebruik T2", "kWh", period);
      break;
    case 2: // returned T1/T2
      theGraph("R1", "R2", "Elektriciteit retour T1", "Elektriciteit retour T2",
               "kWh", period);
      break;
    case 3: // Total E
      theGraph("TE", "TR", "Elektriciteit totaal gebruik",
               "Elektriciteit totaal retour", "kWh", period);
      break;
    case 4: // Gas
      theGraph("G", "", "Gas", "", "kubieke meter", period);
      break;
    default:
      break;
    }
  }
}

void sendStatistics(const String &type, const String &period) {
  Statistics statistics(FST);
  StatisticsStreamServer statStream(server, type, period);

  if (period == "day") { // past 24 to 48 hours
    statistics.sendHours(statStream);
  } else if (period == "week") { // 7 days
    timeManager.printTimeStatus();
    time_t startOfToday = previousMidnight(now());
    // Need 8 records, to calculate 7 day differences
    time_t startOfWeekBefore = startOfToday - (8 * SECS_PER_DAY);
    statistics.sendDays(statStream, startOfWeekBefore, startOfToday);
  } else if (period == "month") { // Same day previous month to now (+- 30 days)
    time_t from = getSameDayPreviousMonth();
    time_t to = createTime(year(), month(), day());
    statistics.sendDays(statStream, from, to);
  } else if (period == "year") { // This calendar year
    time_t from = createTime(year(), 1, 1);
    time_t to = createTime(year() + 1, 1, 1);

    // sendWeeks / sendMonths
    statistics.sendDays(statStream, from, to);
  }
}

void theGraph(const char *type1, const char *type2, String title1,
              String title2, String label, String period) {
  monitoring = false;
  String str = "";
  String pageTitle = "";

  String xAxisFormat = "";
  if (period == "day") {
    xAxisFormat = "HH:mm"; // 14:00
  } else if (period == "week") {
    xAxisFormat = "dddd";  // Donderdag
  } else if (period == "month") {
    xAxisFormat = "d MMMM"; // 6 Januari
  } else if (period == "year") {
    xAxisFormat = "d MMMM yyyy"; // 6 Januari 2024
  }

  addGraphHead(str);
  str += F("<script type=\"text/javascript\" "
           "src=\"https://www.gstatic.com/charts/loader.js\"></script>");
  str += F("<script type=\"text/javascript\">google.charts.load('current', "
           "{'packages':['corechart']});");
  str += F("google.charts.setOnLoadCallback(drawChart1);");
  if (type2[0] != '\0') {
    str += F("google.charts.setOnLoadCallback(drawChart2);");
  }
  str += F("function drawChart1() {");
  str += F("var rawData = [");
  server.sendContent(str);

  delay(200);
  sendStatistics(String(type1), period);
  delay(200);

  str = F("];");
  str += F("var total = rawData.length ? rawData[rawData.length - 1][1] - rawData[0][1] : 0;");
  str += F("document.getElementById('total-1').innerText = total.toFixed(3);");
  str += F("var data = new google.visualization.DataTable();");
  str += F("data.addColumn('date', 'Uur');");
  str += F("data.addColumn('number', '") + label + F("');");
  if (type1[0] == 'T') {
    // Convert timestap to Date object
    str += F("var rows = rawData.map((currentValue) => [new Date(currentValue[0]*1000), currentValue[1]] );");
  } else {
    // Calculate the difference between 2 consecutive records
    str += F("var prevDate = null;");
    str += F("var prevValue = null;");
    str += F("var rows = rawData.reduce((accumulator, currentValue, currentIndex) => {");
    str += F("if (currentIndex !== 0) { accumulator.push([new Date(prevDate*1000), currentValue[1] - prevValue]);}");
    str += F("prevDate = currentValue[0];");
    str += F("prevValue = currentValue[1];");
    str += F("return accumulator;");
    str += F("}, []);");
  }

  // Add the data to the chart
  str += F("data.addRows(rows);");
  server.sendContent(str);

  // TODO: Date format
  str = F("var options = {title: '") + title1 + F("', hAxis: {format: '") + xAxisFormat + F("'}};");
  if (type1[0] == 'T')
    str +=
        F("var chart = new "
          "google.visualization.LineChart(document.getElementById('Chart1'));");
  else
    str += F(
        "var chart = new "
        "google.visualization.ColumnChart(document.getElementById('Chart1'));");
  str += F("chart.draw(data, options); }");
  server.sendContent(str);
  // Log.verboseln(str);
  delay(200);

  str = "";

  if (type2[0] != '\0') {
    str += F("function drawChart2() {");
    str += F("var rawData = [");
    server.sendContent(str);

    delay(200);
    sendStatistics(String(type2), period);
    delay(200);

    str = F("];");
    str += F("var total = rawData.length ? rawData[rawData.length - 1][1] - rawData[0][1] : 0;");
    str += F("document.getElementById('total-2').innerText = total.toFixed(3);");
    str += F("var data = new google.visualization.DataTable();");
    str += F("data.addColumn('date', 'Uur');");
    str += F("data.addColumn('number', '") + label + F("');");
    if (type1[0] == 'T') {
      // Convert timestap to Date object
      str += F("var rows = rawData.map((currentValue) => [new Date(currentValue[0]*1000), currentValue[1]] );");
    } else {
      // Calculate the difference between 2 consecutive records
      str += F("var prevDate = null;");
      str += F("var prevValue = null;");
      str += F("var rows = rawData.reduce((accumulator, currentValue, currentIndex) => {");
      str += F("if (currentIndex !== 0) { accumulator.push([new Date(prevDate*1000), currentValue[1] - prevValue]);}");
      str += F("prevDate = currentValue[0];");
      str += F("prevValue = currentValue[1];");
      str += F("return accumulator;");
      str += F("}, []);");
    }

    // Add the data to the chart
    str += F("data.addRows(rows);");
    server.sendContent(str);
    //'Gasverbruik per uur in m^3'
    str = F("var options = {title: '") + title1 + F("', hAxis: {format: '") + xAxisFormat + F("'}};");
    if (type2[0] == 'T')
      str += F(
          "var chart = new "
          "google.visualization.LineChart(document.getElementById('Chart2'));");
    else
      str += F("var chart = new "
               "google.visualization.ColumnChart(document.getElementById('"
               "Chart2'));");
    str += F("chart.draw(data, options); }");
    server.sendContent(str);
    //  Log.verboseln(str);
  } // only if we have a second file to display

  str = F("</script>");
  str += F("</head><body>");
  str += ("<div "
          "style='text-align:left;display:inline-block;color:#000000;width:"
          "80vw;max-width: 1200px;'><div style='text-align:center;color:#000000;'><h2>P1 "
          "wifi-gateway</h2></div><br>");

  str += F("<fieldset><legend><b>");

  if (period == "day") {
    pageTitle = " Vandaag ";
  } else if (period == "week") {
    pageTitle = " Deze week ";
  } else if (period == "month") {
    pageTitle = " Deze maand ";
  } else if (period == "year") {
    pageTitle = " Dit jaar ";
  }
  str += pageTitle;

  str += F("</b></legend>");
  str += F("<div id='Chart1' style='width:100%; max-width:1200px; "
           "height:500px'></div><br>");
  str += F("Totaal deze periode: <span id='total-1'>0</span>");
  if (type2[0] != '\0') {
    str += F("<div id=\"Chart2\" style=\"width:100%; max-width:1200px; "
             "height:500px\"></div><br>");
    str += F("Totaal deze periode: <span id='total-2'>0</span>");
    str += "</br></fieldset>";
  }
  str += F("<form action='/' method='POST'><button class='button "
           "bhome'>Menu</button></form>");
  addFootBare(str);
  server.sendContent(str);
  //  Log.verboseln(str);
  server.sendContent(F(""));
  monitoring = true;
}

void calendarGas() {
  String str = "";
  monitoring = false;

  addGraphHead(str);

  str +=
      F("<script type='text/javascript' "
        "src='https://www.gstatic.com/charts/loader.js'></script><script "
        "type='text/javascript'>google.charts.load('current', "
        "{packages:['calendar']});google.charts.setOnLoadCallback(drawChart);");
  str += F("function drawChart() {");

  str += F("var rawData = [");
  server.sendContent(str);

  sendStatistics(String("Gc"), String("year"));
  delay(200);

  str = F("];");

  str += F("var dataTable = new google.visualization.DataTable();");
  str += F("dataTable.addColumn('date', 'Datum');");
  str += F("dataTable.addColumn('number', 'Gebruik');");
  // Calculate the difference between 2 consecutive records
  str += F("var prevDate = null;");
  str += F("var prevValue = null;");
  str += F("var rows = rawData.reduce((accumulator, currentValue, currentIndex) => {");
  str += F("if (currentIndex !== 0) { accumulator.push([new Date(prevDate*1000), currentValue[1] - prevValue]);}");
  str += F("prevDate = currentValue[0];");
  str += F("prevValue = currentValue[1];");
  str += F("return accumulator;");
  str += F("}, []);");
  // Add the data to the chart
  str += F("dataTable.addRows(rows);");
  server.sendContent(str);

  str = "";
  str +=
      F("var chart = new "
        "google.visualization.Calendar(document.getElementById('calendar_gas'))"
        "; var options = { title: 'Gas gebruiksintensiteit', height: 350};");
  str += F(
      "chart.draw(dataTable, options);}</script></head><body><div "
      "id='calendar_gas' style='width: 800px; height: 350px;'></div><div><form "
      "action='/' method='POST'><button class='button bhome' style='width: "
      "300px' >Menu</button></form></div>");

  addFootBare(str);
  server.sendContent(str);
  // Log.verboseln(str);
  server.sendContent(F(""));

  monitoring = true;
}

#endif // GRAPH
