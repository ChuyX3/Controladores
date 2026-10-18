const profile = [];
let samples = [];
let eventSource = null;
const pointList = document.getElementById("pointList");

function refreshList()
{
    pointList.innerHTML = "";
    profile.forEach((item,index)=> {
        const option = document.createElement("option");
        option.value = index;
        option.text = item.time + " ms -> " + item.value;
        pointList.add(option);
    });
}

function insertPoint()
{
    const time = parseFloat(document.getElementById("timeInput").value);
    const value = parseFloat(document.getElementById("valueInput").value);
    if(isNaN(time) || isNaN(value)) {
        return;
    }
    profile.push(
    {
        time:time,
        value:value
    });
    refreshList();
}

function removePoint()
{
    const idx = pointList.selectedIndex;

    if(idx < 0) {
        return;
    }
    profile.splice(idx,1);
    refreshList();
}

function moveUp()
{
    const idx = pointList.selectedIndex;
    if(idx <= 0) {
        return;
    }

    [
        profile[idx - 1],
        profile[idx]
    ] = [
        profile[idx],
        profile[idx - 1]
    ];
    refreshList();
    pointList.selectedIndex =
        idx - 1;
}

function moveDown()
{
    const idx = pointList.selectedIndex;

    if(idx < 0) {
        return;
    }

    if(idx >= profile.length - 1) {
        return;
    }

    [
        profile[idx + 1],
        profile[idx]
    ] = [
        profile[idx],
        profile[idx + 1]
    ];

    refreshList();
    pointList.selectedIndex = idx + 1;
}

async function startExperiment()
{
    samples = [];

    await fetch("/start",
        {
            method:"POST",
            headers:
            {
                "Content-Type":"application/json"
            },
            body:JSON.stringify(profile)
        }
    );

    if(eventSource) {
        eventSource.close();
    }

    eventSource = new EventSource("/sample");

    eventSource.addEventListener(
        "newdata",
        function(event)
        {
            const data = JSON.parse(event.data);
            samples.push(data);
            document.getElementById("tValue").textContent = data.t;
            document.getElementById("uValue").textContent = data.u;
            document.getElementById("yValue").textContent = data.y;
            document.getElementById("statusText").textContent = "Ejecutando";
        }
    );

    eventSource.addEventListener(
        "statechanged",
        function(event) {
            const data = JSON.parse(event.data);
            if(data.State === 3) {   // Waiting
                finishExperiment();
            }
        });
}

async function endExperiment()
{
    await fetch("/stop");

    finishExperiment();
}

function finishExperiment()
{
    if(eventSource)
    {
        eventSource.close();
        eventSource = null;
    }

    document.getElementById("statusText")
        .textContent =
        "Finalizado";

    downloadCSV();
}

function downloadCSV()
{
    let csv =
        "t,u,y\r\n";

    samples.forEach(sample =>
    {
        csv +=
            sample.t + "," +
            sample.u + "," +
            sample.y + "\r\n";
    });

    const blob =
        new Blob(
            [csv],
            {
                type:"text/csv"
            });

    const url =
        window.URL.createObjectURL(blob);

    const a =
        document.createElement("a");

    a.href = url;

    a.download =
        "experimento.csv";

    document.body.appendChild(a);

    a.click();

    document.body.removeChild(a);

    window.URL.revokeObjectURL(url);
}

function onLoad()
{
    document.getElementById("insertBtn").addEventListener("click", insertPoint);
    document.getElementById("removeBtn").addEventListener("click", removePoint);
    document.getElementById("upBtn").addEventListener("click", moveUp);
    document.getElementById("downBtn").addEventListener("click", moveDown);
    document.getElementById("startBtn").addEventListener("click", startExperiment);
    document.getElementById("endBtn").addEventListener("click", endExperiment);
}

window.onload=onLoad;