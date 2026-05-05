cmake_minimum_required(VERSION 3.19)
project(OnlineShopping LANGUAGES CXX)
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)
qt_standard_project_setup()
qt_add_executable(OnlineShopping
    WIN32 MACOSX_BUNDLE
    main.cpp
    LoginWindow.h
    LoginWindow.cpp
    discount.h
    discount.cpp
    FileManager.h
    FileManager.cpp
    Inventory.h
    Inventory.cpp
    notification.h
    notification.cpp
    NotificationObserver.h
    NotificationObserver.cpp
    OrderCommand.h
    OrderCommand.cpp
    payment.h
    payment.cpp
    product.h
    product.cpp
    ProductFactory.h
    ProductFactory.cpp
    ShoppingCart.h
    ShoppingCart.cpp
    SystemController.h
    SystemController.cpp
    User.h
    User.cpp
    CartWindow.h
    CartWindow.cpp
    StoreWindow.h
    StoreWindow.cpp
    NotifPanel.h
    NotifPanel.cpp
    ReceiptWindow.h
    ReceiptWindow.cpp
    AuthManager.h
    AuthManager.cpp
)
target_link_libraries(OnlineShopping
    PRIVATE
        Qt::Core
        Qt::Widgets
)
install(TARGETS OnlineShopping
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
qt_generate_deploy_app_script(
    TARGET OnlineShopping
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})