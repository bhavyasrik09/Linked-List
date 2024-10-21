#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

// Define the structure for a book
typedef struct Book {
    int id;
    char title[100];
    char author[100];
    struct Book* next;
} Book;

// Structure to hold references to entry widgets and listbox
typedef struct {
    GtkWidget *title_entry;
    GtkWidget *author_entry;
    GtkWidget *id_entry;
    GtkWidget *listbox;
} EntryWidgets;

Book* bookList = NULL;

// Function prototypes
Book* createBook(int id, char* title, char* author);
void addBook(int id, char* title, char* author);
void displayBooks(GtkWidget *listbox);
void on_add_book(GtkWidget *widget, gpointer data);
void on_delete_book(GtkWidget *widget, gpointer data);
void clearBookList();
void deleteBookByID(int id);

// Main function
int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *window;
    GtkWidget *vbox;
    GtkWidget *add_button;
    GtkWidget *delete_button;
    GtkWidget *title_entry;
    GtkWidget *author_entry;
    GtkWidget *id_entry;
    GtkWidget *listbox;

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Library Management System");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    title_entry = gtk_entry_new();
    author_entry = gtk_entry_new();
    id_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(title_entry), "Enter Book Title");
    gtk_entry_set_placeholder_text(GTK_ENTRY(author_entry), "Enter Book Author");
    gtk_entry_set_placeholder_text(GTK_ENTRY(id_entry), "Enter Book ID");

    gtk_box_pack_start(GTK_BOX(vbox), title_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), author_entry, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), id_entry, FALSE, FALSE, 0);

    add_button = gtk_button_new_with_label("Add Book");
    delete_button = gtk_button_new_with_label("Delete Book");

    gtk_box_pack_start(GTK_BOX(vbox), add_button, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), delete_button, FALSE, FALSE, 0);

    listbox = gtk_list_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), listbox, TRUE, TRUE, 0);

    // Create an instance of EntryWidgets
    EntryWidgets entries = {title_entry, author_entry, id_entry, listbox};

    g_signal_connect(add_button, "clicked", G_CALLBACK(on_add_book), &entries);
    g_signal_connect(delete_button, "clicked", G_CALLBACK(on_delete_book), id_entry);
    
    // Set entries as user data for the delete button
    g_object_set_data(G_OBJECT(delete_button), "entries", &entries);

    gtk_widget_show_all(window);
    gtk_main();

    return 0;
}

// Function to create a new book
Book* createBook(int id, char* title, char* author) {
    Book* newBook = (Book*)malloc(sizeof(Book));
    newBook->id = id;
    strcpy(newBook->title, title);
    strcpy(newBook->author, author);
    newBook->next = NULL;
    return newBook;
}

// Function to add a book to the linked list
void addBook(int id, char* title, char* author) {
    Book* newBook = createBook(id, title, author);
    newBook->next = bookList;
    bookList = newBook;
}

// Function to display all books in the listbox
void displayBooks(GtkWidget *listbox) {
    // Remove all existing children in the listbox
    GList *children = gtk_container_get_children(GTK_CONTAINER(listbox));
    GList *iter;

    // Iterate through all children and remove them
    for (iter = children; iter != NULL; iter = iter->next) {
        gtk_widget_destroy(GTK_WIDGET(iter->data));
    }

    g_list_free(children);  // Free the list after removing all children

    // Add the books to the listbox
    Book* temp = bookList;
    while (temp != NULL) {
        char buffer[256];
        sprintf(buffer, "ID: %d, Title: %s, Author: %s", temp->id, temp->title, temp->author);
        GtkWidget *label = gtk_label_new(buffer);
        gtk_list_box_insert(GTK_LIST_BOX(listbox), label, -1);
        temp = temp->next;
    }

    gtk_widget_show_all(listbox);  // Show all widgets in the listbox
}

// Function to handle adding a book
void on_add_book(GtkWidget *widget, gpointer data) {
    EntryWidgets *entries = (EntryWidgets *)data;
    const gchar *title = gtk_entry_get_text(GTK_ENTRY(entries->title_entry));
    const gchar *author = gtk_entry_get_text(GTK_ENTRY(entries->author_entry));
    int id = atoi(gtk_entry_get_text(GTK_ENTRY(entries->id_entry)));

    if (id <= 0) {
        g_print("Invalid ID\n");
        return;
    }

    addBook(id, (char *)title, (char *)author);
    displayBooks(entries->listbox); // Pass the listbox directly
}

// Function to handle deleting a book
void on_delete_book(GtkWidget *widget, gpointer data) {
    const gchar *id_text = gtk_entry_get_text(GTK_ENTRY(data));
    int id = atoi(id_text);
    
    // Try to delete the book
    deleteBookByID(id);

    // Clear the entry field
    gtk_entry_set_text(GTK_ENTRY(data), "");

    // Refresh the list box display
    displayBooks(((EntryWidgets*)g_object_get_data(G_OBJECT(widget), "entries"))->listbox);
}

// Function to delete a book by ID
void deleteBookByID(int id) {
    Book *current = bookList;
    Book *previous = NULL;

    while (current != NULL) {
        if (current->id == id) {
            if (previous == NULL) {
                bookList = current->next; // Delete head
            } else {
                previous->next = current->next; // Delete current
            }
            free(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

// Function to clear the book list
void clearBookList() {
    Book *current = bookList;
    while (current != NULL) {
        Book *next = current->next;
        free(current);
        current = next;
    }
    bookList = NULL;
}
