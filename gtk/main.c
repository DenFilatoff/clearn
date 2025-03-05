#include <gtk/gtk.h>

G_GNUC_BEGIN_IGNORE_DEPRECATIONS enum {
    FILENAME_COLUMN,
    N_COLUMNS
};

gboolean populate_tree_model(GtkTreeStore *store, GFile *file)
{
    GError *error;
    static GtkTreeIter iter1;
    static GtkTreeIter iter2;
    GFileType filetype;
    GFileEnumerator *enumerator = NULL;
    GFileInfo *fileinfo = NULL;
    const char *relative_path;
    char *filename = NULL;
    char *basename = NULL;

    filetype = g_file_query_file_type(file, G_FILE_QUERY_INFO_NONE, NULL);
    if (filetype == G_FILE_TYPE_DIRECTORY) {
        gtk_tree_store_append(store, &iter1, NULL);
        filename = g_file_get_parse_name(file);
        gtk_tree_store_set(store, &iter1, FILENAME_COLUMN,  filename, -1);
        g_free(filename);
        enumerator =
            g_file_enumerate_children(file,
                          G_FILE_ATTRIBUTE_STANDARD_NAME,
                          G_FILE_QUERY_INFO_NONE, NULL,
                          &error);
        for (fileinfo =
             g_file_enumerator_next_file(enumerator, NULL, &error);
             fileinfo != NULL;
             fileinfo = g_file_enumerator_next_file(enumerator, NULL, &error)) {
            relative_path = g_file_info_get_name(fileinfo);
            populate_tree_model(store, g_file_resolve_relative_path(file, relative_path));
            g_object_unref(fileinfo);
        }
        g_file_enumerator_close (enumerator, NULL, &error);
        g_object_unref(enumerator);
    }
    else if (filetype == G_FILE_TYPE_REGULAR) {
        gtk_tree_store_append(store, &iter2, &iter1);
        basename = g_file_get_basename(file);
        gtk_tree_store_set(store, &iter2, FILENAME_COLUMN, basename, -1);
        g_free(basename);
    }
    return TRUE;
}

gint main (gint argc, gchar *argv[]){
    GtkWidget *window;
    GtkWidget *box;
    GtkTreeStore *store;
    GtkWidget *tree;
    GtkTreeViewColumn *column;
    GtkCellRenderer *renderer;
    GFile *fl;
    gtk_init (&argc, &argv);
    window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
    store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING);
    fl = g_file_new_for_path(".");
    populate_tree_model(store, fl);
    g_object_unref(fl);
    tree = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(G_OBJECT(store));
    renderer = gtk_cell_renderer_text_new();
    g_object_set(G_OBJECT(renderer), "foreground", "blue", NULL);
    column = gtk_tree_view_column_new_with_attributes("Filename", renderer,
                              "text",
                              FILENAME_COLUMN,
                              NULL);

    gtk_tree_view_append_column(GTK_TREE_VIEW(tree), column);
    gtk_container_add (GTK_CONTAINER (window), box);
    gtk_container_add (GTK_CONTAINER (box), tree);
    gtk_window_set_default_size (GTK_WINDOW (window), 800, 600);
    gtk_widget_show_all (window);
    gtk_main ();

    return 0;
}
