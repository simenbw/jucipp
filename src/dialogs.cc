#include "dialogs.h"
#include "juci.h"
#include "singletons.h"
#include <cmath>

namespace sigc {
#ifndef SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
  template <typename Functor>
  struct functor_trait<Functor, false> {
    typedef decltype (::sigc::mem_fun(std::declval<Functor&>(),
                                      &Functor::operator())) _intermediate;
    typedef typename _intermediate::result_type result_type;
    typedef Functor functor_type;
  };
#else
  SIGC_FUNCTORS_DEDUCE_RESULT_TYPE_WITH_DECLTYPE
#endif
}

Dialog::Message::Message(const std::string &text): Gtk::MessageDialog(text, false, Gtk::MessageType::MESSAGE_INFO, Gtk::ButtonsType::BUTTONS_CLOSE, true) {
  auto g_application=g_application_get_default();
  auto gio_application=Glib::wrap(g_application, true);
  auto application=Glib::RefPtr<Application>::cast_static(gio_application);
  set_transient_for(*application->window);
  set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
  
  show_now();
  
  while(g_main_context_pending(NULL))
    g_main_context_iteration(NULL, false);
}

std::string Dialog::gtk_dialog(const std::string &title,
                        const std::vector<std::pair<std::string, Gtk::ResponseType>> &buttons,
                        Gtk::FileChooserAction gtk_options,
                        const std::string &file_name) {
  Gtk::FileChooserDialog dialog(title, gtk_options);
  
  auto g_application=g_application_get_default(); //TODO: Post issue that Gio::Application::get_default should return pointer and not Glib::RefPtr
  auto gio_application=Glib::wrap(g_application, true);
  auto application=Glib::RefPtr<Application>::cast_static(gio_application);
  dialog.set_transient_for(*application->window);
  
  auto current_path=application->window->notebook.get_current_folder();
  if(current_path.empty())
    current_path=boost::filesystem::current_path();
  gtk_file_chooser_set_current_folder((GtkFileChooser*)dialog.gobj(), current_path.string().c_str());

  if (!file_name.empty())
    gtk_file_chooser_set_filename((GtkFileChooser*)dialog.gobj(), file_name.c_str());
  dialog.set_position(Gtk::WindowPosition::WIN_POS_CENTER_ON_PARENT);
  
  for (auto &button : buttons) 
    dialog.add_button(button.first, button.second);
  return dialog.run() == Gtk::RESPONSE_OK ? dialog.get_filename() : "";
}
