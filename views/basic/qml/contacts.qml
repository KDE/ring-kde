/*
 *   Copyright 2018 Fabian Riethmayer
 *   Copyright 2019 Emmanuel Lepage <emmanuel.lepage@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 3, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU Library General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
import QtQuick 2.6

ListModel {
    id: model
    // Pattern
    ListElement {
        firstname: ""
        lastname: ""
        image: ""
        communication: [
            ListElement {
                icon: ""
                text: "+12 34 1234 5678"
                description: "mobile"
                type: "phone"
                actions: [
                    ListElement {
                        icon: ""
                    }
                ]
            }
        ]
    }

    Component.onCompleted: {
        model.clear()
        var data = [{
                        "firstname": "Berna",
                        "lastname": "Hicks",
                        "image": "image://icon/im-user",
                        "communication": [{
                                "icon": "call-start",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "Mobile private",
                                "default": true,
                                "actions": [{
                                        "icon": "kmouth-phrase-new",
                                        "text": "SMS"
                                    }]
                            }, {
                                "icon": "",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "VOIP"
                            }, {
                                "type": "email",
                                "icon": "mail-message",
                                "text": "susan@kde.org",
                                "default": true,
                                "description": "E-Mail private"
                            }],
                        "history": [{
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            }, {
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            },{
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            }]
                    }, {
                        "firstname": "Matt",
                        "lastname": "Byrne",
                        "image": "image://icon/im-user"
                    }, {
                        "firstname": "Santiago",
                        "lastname": "Thorne",
                        "image": "image://icon/im-user",
                        "communication": [{
                                "icon": "call-start",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "Mobile private",
                                "default": true,
                                "actions": [{
                                        "icon": "kmouth-phrase-new",
                                        "text": "SMS"
                                    }]
                            }, {
                                "icon": "",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "VOIP"
                            }, {
                                "type": "email",
                                "icon": "mail-message",
                                "text": "susan@kde.org",
                                "default": true,
                                "description": "E-Mail private"
                            }]
                    }, {
                        "firstname": "Susan",
                        "lastname": "Hoffman",
                        "image": "image://icon/im-user",
                        "communication": [{
                                "icon": "call-start",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "Mobile private",
                                "default": true,
                                "actions": [{
                                        "icon": "kmouth-phrase-new",
                                        "text": "SMS"
                                    }]
                            }, {
                                "icon": "",
                                "type": "phone",
                                "text": "+12 34 1234 5678",
                                "description": "VOIP"
                            }, {
                                "type": "email",
                                "icon": "mail-message",
                                "text": "susan@kde.org",
                                "default": true,
                                "description": "E-Mail private"
                            }],
                        "history": [{
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            }, {
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            },{
                                "icon": "call-start",
                                "text": "+12 34 1234 5678",
                                "date": "2018-10-10"
                            }, {
                                "icon": "mail-message",
                                "text": "Lorem ipsum",
                                "date": "2018-10-10"
                            }]
                    }, {
                        "firstname": "Philip",
                        "lastname": "Steele",
                        "image": "image://icon/im-user"
                    }]
        model.insert(0, data)
    }
}
