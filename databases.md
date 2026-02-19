Enhancement Three: Databases

Artifact: Event Planner Android App (CS 360)

Artifact Description

The third artifact is an Event Planner Android application originally developed in CS 360. The app enables users to create, update, and delete events containing details such as event name, date, and description. The project integrates SQLite for persistent data storage and demonstrates full CRUD functionality within a mobile application environment.

Justification for Inclusion and Enhancement

I selected this artifact because it highlights database design and data management within a practical application. While the original implementation supported basic event management, it did not associate events with individual users.

The enhancement introduced a relational improvement by adding a user_id column to the events table, thereby linking events to specific users. This required modifying the database schema within DatabaseHelper.java and updating insertion, update, and retrieval methods to ensure correct filtering and data integrity. These changes demonstrate the implementation of relational database principles and structured data modeling within a mobile context.

Course Outcomes Addressed

This enhancement aligns with course outcomes related to database design, structured query implementation, and secure data management. By modifying the schema and updating CRUD operations, I demonstrated an understanding of relational relationships and the importance of maintaining data integrity. The project also reflects professional development skills in enhancing and extending existing systems rather than rebuilding from scratch.

Reflection on the Enhancement Process

Through this enhancement, I gained practical experience modifying a live database schema and ensuring that application logic remained consistent after structural changes. A key challenge involved maintaining compatibility between the user interface and updated data queries. This process reinforced the importance of synchronization between front-end interaction and back-end storage logic. Overall, this artifact demonstrates my ability to apply database principles effectively while considering both system functionality and user experience.
