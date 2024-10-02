// Get elements
const taskInput = document.getElementById("task-input");
const addTaskBtn = document.getElementById("add-task-btn");
const taskList = document.getElementById("task-list");

// Event listener for adding a task
addTaskBtn.addEventListener("click", addTask);
taskInput.addEventListener("keydown", (event) => {
    event.key == "Enter" ? addTask() : "";
});

body.addEventListener("DOMConetenLoaded", loadTasks);

// Function to add a task
function addTask(task) {
    const taskText = task ?? taskInput.value.trim();
    if (taskText === "") {
        alert("Please enter a task.");
        return;
    }

    // Create list item
    const li = document.createElement("li");

    // Create checkbox
    const checkbox = document.createElement("input");
    checkbox.type = "checkbox";
    checkbox.addEventListener("change", toggleTask);

    // Create task text
    const span = document.createElement("span");
    span.textContent = " " + taskText;

    // Create delete button
    const deleteBtn = document.createElement("button");
    deleteBtn.innerHTML = "&times;";
    deleteBtn.className = "delete-btn";
    deleteBtn.addEventListener("click", deleteTask);

    // Append elements
    li.appendChild(checkbox);
    li.appendChild(span);
    li.appendChild(deleteBtn);
    taskList.appendChild(li);

    // Clear input
    taskInput.value = "";
    addTaskToDB(taskText);
}

function addTaskToDB(taskText) {
    fetch("tasks", {
        method: "POST",
        body: `{${taskText}}`,
        headers: {
            "Content-type": "application/text; charset=UTF-8",
        },
    });
}

// Function to toggle task completion
function toggleTask() {
    const li = this.parentElement;
    if (this.checked) {
        li.classList.add("completed");
    } else {
        li.classList.remove("completed");
    }
}

// Function to delete a task
function deleteTask() {
    const li = this.parentElement;
    taskList.removeChild(li);
}

async function loadTasks() {
    let tasks = await fetch("tasks", {
        method: "GET"
    })
    tasks = tasks.split(" ; ");
    tasks.forEach(task => addTask(task));
}
