
var a = new Array(10);
a.fill(0);
var arr = [];

var wb = XLSX.utils.book_new();

wb.Props = {
    Title: "SheetJS Tutorial",
    Subject: "Test",
    Author: "Red Stapler",
    CreatedDate: new Date(2017, 12, 19)
};

wb.SheetNames.push("Test Sheet");



//Helper function for making excel sheet
function s2ab(s) {
    //alert(1);
    //s = s.toString();
    //alert(s.length);
    var buf = new ArrayBuffer(s.length);
    var view = new Uint8Array(buf);
    for (var i = 0; i < s.length; i++) view[i] = s.charCodeAt(i) & 0xFF;
    return buf;

}

//Set the value of the array corresponding to the button to 1
function hold(b){
   a[b - 1] = 1;
    return false;
}

//Add the current values to the final array
function addData() {
    //THe last element of the array is the value we input to say if the thing was a seven or not
    a[9] = document.getElementById("is7").value;

    if ((a[9] != 1 && a[9] != 0)) {
        a[9] = 0;
        alert("Enter a 1 or 0 in the text box");
        return;
    }
    //Remove leading 0s
    a[9] = parseInt(a[9], 10);

    
    arr.push(a.splice(0));

    a = new Array(10);
    a.fill(0);

    return;
}

//Send the entire array of values to an excel sheet
function pushToExcel() {

    //THe last element of the array is the value we input to say if the thing was a seven or not
    a[9] = document.getElementById("is7").value;

    if ((a[9] != 1 && a[9] != 0)) {
        a[9] = 0;
        alert("Enter a 1 or 0 in the text box");
        return;
    }
    //Remove leading 0s
    a[9] = parseInt(a[9], 10);

    var ws_data = arr;
    var ws = XLSX.utils.aoa_to_sheet(ws_data);

    wb.Sheets["Test Sheet"] = ws;
    var wbout = XLSX.write(wb, { bookType: 'xlsx', type: 'binary' });

    saveAs(new Blob([s2ab(wbout)], { type: "application/octet-stream" }), 'test1.xlsx');
    
    return false;
}

//Show the current array that would be pushed to excel.
function showArray() {

    alert(arr);
}


function showCurrentValues() {
    //THe last element of the array is the value we input to say if the thing was a seven or not
    a[9] = document.getElementById("is7").value;

    if ((a[9] != 1 && a[9] != 0)) {
        a[9] = 0;
        alert("Enter a 1 or 0 in the text box");
        return;
    }
    //Remove leading 0s
    a[9] = parseInt(a[9], 10);

    alert(a);
}