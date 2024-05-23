# File_System
This project delves into system programming to create a user-friendly application named inotifywait that leverages the pipes and forks in Linux to monitor a specified folder for modifications. When changes occur (file creation, deletion, or modification), the application promptly informs the user, providing a valuable tool for system administrators and developers to stay abreast of file system activity.

Functionality

Folder Monitoring: The application continuously monitors a user-specified folder for file system events.
Event Detection: It detects various file system events, including:

Creation of new files within the folder

Deletion of existing files

Modifications to files (e.g., changes in content, timestamps)

User Notification: Upon detecting an event, the application alerts the user through a visually distinct notification on the terminal.
